#include "SocksConnection.h"
#include "states/InitialState.h"
#include "decorators/QIODeviceDecorator.h"
#include "decorators/ThrottlingDecorator.h"

#include <QtDebug>
#include <QHostAddress>


SocksConnection::SocksConnection(QAbstractSocket *socket, QObject *parent)
    : QObject(parent)
    , _rawSocket(socket)
    , _socksVersionSet(false)
{
    if (_rawSocket.isNull())
    {
        qWarning() << this << "initialized with null socket";
        return;
    }

    _socket = new ThrottlingDecorator(_rawSocket,this);

    //When we have incoming bytes, we read them
    connect(_socket.data(), &QIODevice::readyRead, this, &SocksConnection::onReadyRead);

    //When our socket closes, we die
    connect(_socket.data(), &QIODevice::aboutToClose, this, &SocksConnection::deleteLater);

    //Set our state to the initial "someone just connected" state
    setState(new InitialState(this));
}

SocksConnection::~SocksConnection()
{
    if (!_socket.isNull())
    {
        _socket->close();
        _socket->deleteLater();
    }

    if (!_connectionState.isNull())
        delete _connectionState;
}

QPointer<SocksState> SocksConnection::connectionState()
{
    return _connectionState;
}

SocksProtocolMessage::SocksVersion SocksConnection::socksVersion() const
{
    return _socksVersion;
}

void SocksConnection::setSocksVersion(SocksProtocolMessage::SocksVersion nVer)
{
    if (_socksVersionSet)
        return;

    _socksVersionSet = true;
    _socksVersion = nVer;
}

bool SocksConnection::socksVersionSet() const
{
    return _socksVersionSet;
}

bool SocksConnection::sendMessage(QSharedPointer<SocksProtocolMessage> msg, QString *error)
{
    if (msg.isNull())
    {
        if (error)
            *error = "Cannot send null message";
        return false;
    }

    QByteArray toSend;
    QString serializeError;
    if (!msg->toBytes(&toSend,&serializeError))
    {
        if (error)
            *error = "Failed to send message. Error:" + serializeError;
        return false;
    }

    this->sendData(toSend);
    return true;
}

QHostAddress SocksConnection::myBoundAddress() const
{
    return _rawSocket->localAddress();
}

QHostAddress SocksConnection::peerAddress() const
{
    return _rawSocket->peerAddress();
}

void SocksConnection::sendData(const QByteArray &toSend)
{
    if (_socket.isNull())
    {
        qWarning() << "Tried to send" << toSend.size() << "bytes but socket was null";
        return;
    }

    qint64 written = _socket->write(toSend);
    if (written != toSend.size())
        qWarning() << this << "wrote" << written << "bytes but should have written" << toSend.size() << "bytes";

    //qDebug() << "State" << _connectionState << "wrote" << toSend.toHex();
}

void SocksConnection::setState(SocksState *state)
{
    if (state == nullptr)
        return;

    if (!_connectionState.isNull())
        delete _connectionState;

    _connectionState = state;

    //Call the "you've just been set as the new state!" handler
    state->handleSetAsNewState();

    //Force the new state to start off where the last one ended traffic-wise
    if (!_recvBuffer.isEmpty())
        this->handleIncomingBytes(_recvBuffer);
}

void SocksConnection::close()
{
    if (_socket.isNull())
        return;

    _socket->close();
}

void SocksConnection::handleIncomingBytes(QByteArray &bytes)
{
    if (_connectionState.isNull())
    {
        qWarning() << this << "No state to do handleIncomingBytes";
        return;
    }

    if (bytes.size() == 0)
    {
        qWarning() << this << "got empty array in handleIncomingBytes";
        return;
    }

    //Pass the bytes to the state by reference. The state will eat the bytes it wants
    _connectionState->handleIncomingBytes(bytes);
}

void SocksConnection::onReadyRead()
{
    if (_socket.isNull())
        return;

    int count = 0;
    const int kMaxIterations = 50;

    while (_socket->bytesAvailable() > 0 && ++count < kMaxIterations)
        _recvBuffer.append(_socket->readAll());

    if (count == kMaxIterations)
        qDebug() << this << "looped too much";

    //Send the whole buffer to the state, which should eat the portions it wants
    handleIncomingBytes(_recvBuffer);
}

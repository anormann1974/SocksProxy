#include "SocksConnection.h"

#include <QLoggingCategory>
#include <QHostAddress>
#include <QTcpSocket>

namespace {
static QLoggingCategory lc("SocksConnection");
}

using namespace std::chrono_literals;


SocksConnection::SocksConnection(QAbstractSocket *socket, QObject *parent)
    : QObject(parent)
    , _clientSocket(socket)
    , _state(Connected)
    , _timeoutPeriod{30s}
{
    if (_clientSocket.isNull())
    {
        qCWarning(lc) << "Initialized with null socket";
        return;
    }

    //When we have incoming bytes, we read them
    connect(_clientSocket, &QIODevice::readyRead, this, &SocksConnection::onReadyRead);

    //When our socket closes, we die
    connect(_clientSocket, &QIODevice::aboutToClose, this, &SocksConnection::deleteLater);

    connect(&_timeout, &QTimer::timeout, this, &SocksConnection::close);
}

SocksConnection::~SocksConnection()
{
    qCDebug(lc) << "Closing connection to:" << _remoteHost << "timeout:" << (_timeout.remainingTime() == _timeout.interval());

    if (!_remoteSocket.isNull())
    {
        _remoteSocket->close();
        _remoteSocket->deleteLater();
    }

    if (!_clientSocket.isNull())
    {
        _clientSocket->close();
        _clientSocket->deleteLater();
    }    
}

void SocksConnection::close()
{
    if (_clientSocket.isNull())
        return;

    _clientSocket->close();
}

void SocksConnection::onReadyRead()
{
    if (_clientSocket.isNull())
        return;

    QByteArray recvBuffer;

    while (_clientSocket->bytesAvailable() > 0)
        recvBuffer.append(_clientSocket->readAll());

    handleIncomingBytes(recvBuffer);
}

void SocksConnection::connectToHost(const QHostAddress &host, quint16 port)
{
    qCDebug(lc).noquote().nospace() << "Connecting to: " << host.toString() << ":" << port;

    _remoteSocket = new QTcpSocket(this);

    connect(_remoteSocket, &QTcpSocket::aboutToClose, this, &SocksConnection::close);
    connect(_remoteSocket, &QTcpSocket::errorOccurred, _remoteSocket, &QTcpSocket::close);
    connect(_remoteSocket, &QTcpSocket::errorOccurred, this, &SocksConnection::close);
    connect(_remoteSocket, &QTcpSocket::connected, this, &SocksConnection::onRemoteConnected);
    connect(_remoteSocket, &QTcpSocket::readyRead, this, &SocksConnection::onRemoteReadyRead);

    _remoteSocket->connectToHost(host, port);
}

void SocksConnection::onRemoteReadyRead()
{
    while (_remoteSocket->bytesAvailable() > 0)
    {
        const auto bytes = _remoteSocket->readAll();

        _clientSocket->write(bytes);
    }

    _timeout.start(_timeoutPeriod);
}

void SocksConnection::onRemoteConnected()
{
    _remoteHost = _remoteSocket->peerAddress().toString();
    _timeout.start(_timeoutPeriod);

    handleRemoteConnected();
}

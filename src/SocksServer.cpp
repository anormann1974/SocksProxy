#include "SocksServer.h"
#include "SocksConnection.h"

#include <QLoggingCategory>
#include <QTcpServer>
#include <QDataStream>


namespace {
static QLoggingCategory lc("SocksServer");
}

SocksServer::SocksServer(QHostAddress listenAddress,
                         quint16 listenPort,
                         qreal throttle,
                         QObject *parent)
    : QObject(parent)
    , _listenAddress(listenAddress)
    , _listenPort(listenPort)
    , _throttle(throttle)
{
}

SocksServer::~SocksServer()
{
    qCDebug(lc) << "Shutting down";

    if (!_serverSock.isNull())
    {
        _serverSock->close();
    }

    for (auto conn : _connections)
    {
        if (!conn.isNull())
        {
            conn->close();
            conn->deleteLater();
        }
    }

    _connections.clear();
}

void SocksServer::start()
{
    if (!_serverSock.isNull())
        _serverSock->deleteLater();

    _serverSock.reset(new QTcpServer(this));

    if (!_serverSock->listen(_listenAddress,_listenPort))
    {
        _serverSock->deleteLater();
        qCWarning(lc) << "Failed to listen on" << _listenAddress << _listenPort;
        return;
    }

    connect(_serverSock.data(), &QTcpServer::newConnection, this, &SocksServer::onNewIncomingConnection);

    qCDebug(lc) << "Listening on" << _listenAddress << _listenPort;
}

bool SocksServer::isStarted() const
{
    if (_serverSock.isNull())
        return false;

    return _serverSock->isListening();
}

void SocksServer::onNewIncomingConnection()
{
    int count = 0;
    const int max = 50;

    while (_serverSock->hasPendingConnections() && ++count < max)
    {
        auto clientSock = _serverSock->nextPendingConnection();

//        connect(clientSock, &QTcpSocket::readyRead, this, [clientSock]() {
//            QByteArray buffer;

//            while (clientSock->bytesAvailable() > 0)
//            {
//                buffer.append(clientSock->readAll());
//            }

//            qCDebug(lc) << "New data available:" << buffer.toHex(' ');

//            // init state
//            {
//                uint8_t version;
//                QSet<uint8_t> authList;

//                QDataStream stream(&buffer, QIODevice::ReadOnly);
//                stream >> version;

//                if (version == 5)
//                {
//                    uint8_t numAuth;
//                    stream >> numAuth;

//                    authList.reserve(numAuth);

//                    for (uint8_t i=0; i<numAuth; ++i)
//                    {
//                        uint8_t auth;
//                        stream >> auth;

//                        authList.insert(auth);
//                    }

//                    if (authList.contains(0x00))
//                    {
//                        QByteArray output;
//                        QDataStream out(&output, QIODevice::WriteOnly);

//                        const uint8_t ver = 5;
//                        const uint8_t auth = 0;
//                        out << ver << auth;

//                        qCDebug(lc) << "Writing auth reply:" << output.toHex(' ');
//                        clientSock->write(output);
//                    }
//                }
//            }
//        });

//        connect(clientSock, &QTcpSocket::aboutToClose, this, []() {
//        });


        QPointer<SocksConnection> connection = new SocksConnection(clientSock, this);

        connect(connection.data(), &SocksConnection::destroyed, this, [this, connection]() {
            if (connection.isNull())
            {
                //qCDebug(lc) << "Removing connection" << connection.data();
                _connections.removeOne(connection);
            }
        });

        _connections.append(connection);

        qCDebug(lc).nospace().noquote() << "Client " << clientSock->peerAddress().toString() << ":" << clientSock->peerPort() << " connected";
    }

    if (count == max)
        qCDebug(lc) << "Looped too much";
}

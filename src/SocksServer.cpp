#include "SocksServer.h"
#include "Socks4Connection.h"
#include "Socks5Connection.h"

#include <QLoggingCategory>
#include <QTcpServer>
#include <QTcpSocket>
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
    while (_serverSock->hasPendingConnections())
    {
        auto clientSock = _serverSock->nextPendingConnection();

        connect(clientSock, &QTcpSocket::readyRead, this, &SocksServer::onReadReady);

//        connect(clientSock, &QTcpSocket::aboutToClose, this, []() {
//        });


        //qCDebug(lc).nospace().noquote() << "Client " << clientSock->peerAddress().toString() << ":" << clientSock->peerPort() << " connected";
    }
}

void SocksServer::onReadReady()
{
    auto clientSock = qobject_cast<QTcpSocket*>(sender());

    QByteArray buffer;

    while (clientSock->bytesAvailable() > 0)
    {
        buffer.append(clientSock->readAll());
    }

    //qCDebug(lc) << "New data available:" << buffer.toHex(' ') << clientSock;

    QPointer<SocksConnection> connection;

    if (buffer.size() > 0 && buffer.at(0) == 5)
    {
        qCDebug(lc) << "SOCKS5 request";

        connection = new Socks5Connection(clientSock, buffer, this);
    }
    else if (buffer.size() > 0 && buffer.at(0) == 4)
    {
        qCDebug(lc) << "SOCKS4 request";

        connection = new Socks4Connection(clientSock, buffer, this);
    }

    if (!connection.isNull())
    {
        connect(connection, &SocksConnection::destroyed, this, [this, connection]() {
            if (connection.isNull())
            {
                _connections.removeOne(connection);
                qCDebug(lc) << "Removing connection:" << _connections.size();
            }
        });

        _connections.append(connection);
    }

    disconnect(clientSock, &QTcpSocket::readyRead, this, &SocksServer::onReadReady);
}

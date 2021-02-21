#include "SocksServer.h"
#include "SocksConnection.h"

#include <QLoggingCategory>
#include <QTcpServer>


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
        QTcpSocket *clientSock = _serverSock->nextPendingConnection();
        QPointer<SocksConnection> connection = new SocksConnection(clientSock, this);

        connect(connection.data(), &SocksConnection::destroyed, this, [this, connection]() {
            if (connection.isNull())
            {
                //qCDebug(lc) << "Removing connection" << connection.data();
                _connections.removeOne(connection);
            }
        });
        _connections.append(connection);
        //qDebug() << "Client" << clientSock->peerAddress().toString() << ":" << clientSock->peerPort() << "connected";
    }

    if (count == max)
        qCDebug(lc) << "Looped too much";
}

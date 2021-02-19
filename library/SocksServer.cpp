#include "SocksServer.h"

#include <QtDebug>
#include <QTcpServer>
#include <QMutableListIterator>

#include "SocksConnection.h"

SocksServer::SocksServer(QHostAddress listenAddress,
                         quint16 listenPort,
                         qreal throttle,
                         QObject *parent) :
    QObject(parent), _listenAddress(listenAddress), _listenPort(listenPort), _throttle(throttle)
{
}

SocksServer::~SocksServer()
{
    qDebug() << "SocksServer" << this << "shutting down";
    if (!_serverSock.isNull())
    {
        _serverSock->close();
        _serverSock->deleteLater();
    }

    for (QPointer<SocksConnection> conn : _connections)
    {
        if (conn.isNull())
            continue;

        conn->close();
        conn->deleteLater();
    }

    _connections.clear();
}

void SocksServer::start()
{
    if (!_serverSock.isNull())
        _serverSock->deleteLater();

    _serverSock = new QTcpServer(this);

    if (!_serverSock->listen(_listenAddress,_listenPort))
    {
        _serverSock->deleteLater();
        qWarning() << this << "failed to listen on" << _listenAddress << _listenPort;
        return;
    }


    connect(_serverSock.data(),
            &QTcpServer::newConnection,
            this,
            &SocksServer::handleNewIncomingConnection);

    qDebug() << "Listening on" << _listenAddress << _listenPort;
}

bool SocksServer::isStarted() const
{
    if (_serverSock.isNull())
        return false;
    return _serverSock->isListening();
}

//private slot
void SocksServer::handleNewIncomingConnection()
{
    int count = 0;
    const int max = 50;

    while (_serverSock->hasPendingConnections() && ++count < max)
    {
        QTcpSocket *clientSock = _serverSock->nextPendingConnection();
        QPointer<SocksConnection> connection = new SocksConnection(clientSock, this);

        connect(connection.data(),
                &SocksConnection::destroyed,
                this,
                &SocksServer::handleConnectionDestroyed);
        _connections.append(connection);
        //qDebug() << "Client" << clientSock->peerAddress().toString() << ":" << clientSock->peerPort() << "connected";
    }

    if (count == max)
        qDebug() << this << "looped too much";
}

//private slot
void SocksServer::handleConnectionDestroyed()
{
    QMutableListIterator<QPointer<SocksConnection>> iter(_connections);

    bool pruned = false;

    while (iter.hasNext())
    {        
        if (const QPointer<SocksConnection>& conn = iter.next(); conn.isNull())
        {
            iter.remove();
            pruned = true;
        }
    }

    if (!pruned)
        qWarning() << "handleConnectionDestroyed called but no dead connections were found...";
}

#pragma once

#include <QObject>
#include <QTcpServer>
#include <QPointer>
#include <QList>
#include <QHostAddress>

class SocksConnection;


class SocksServer : public QObject
{
    Q_OBJECT
public:
    explicit SocksServer(QHostAddress listenAddress = QHostAddress::Any,
                         quint16 listenPort = 1080,
                         qreal throttle = 500.0,
                         QObject *parent = nullptr);
    ~SocksServer();

    void start();

    bool isStarted() const;
    
private:
    void onNewIncomingConnection();

    QHostAddress _listenAddress;
    quint16 _listenPort;
    qreal _throttle;
    QScopedPointer<QTcpServer> _serverSock;
    QList<QPointer<SocksConnection> > _connections;    
};

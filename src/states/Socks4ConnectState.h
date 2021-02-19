#pragma once

#include "SocksState.h"
#include "protocol/Socks4RequestMessage.h"

#include <QSharedPointer>
#include <QTcpSocket>
#include <QHostInfo>

class Socks4ConnectState : public SocksState
{
    Q_OBJECT
public:
    explicit Socks4ConnectState(QSharedPointer<Socks4RequestMessage> request, SocksConnection *parent = nullptr);

    //pure-virtual from SocksState
    virtual void handleIncomingBytes(QByteArray &bytes);

    //virtual from SocksState
    virtual void handleSetAsNewState();
    

private slots:
    void handleSocketConnected();
    void handleSocketError(QAbstractSocket::SocketError);
    void handleDomainLookupResult(const QHostInfo& info);


private:
    void handleIP();
    void handleDomain();

    QSharedPointer<Socks4RequestMessage> _request;
    QTcpSocket *_socket{nullptr};
    int _dnsLookupID{-1};
};

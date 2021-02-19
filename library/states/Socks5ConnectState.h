#pragma once
#include "SocksState.h"
#include "protocol/Socks5RequestMessage.h"

#include <QSharedPointer>
#include <QTcpSocket>
#include <QHostInfo>


class Socks5ConnectState : public SocksState
{
    Q_OBJECT
public:
    explicit Socks5ConnectState(QSharedPointer<Socks5RequestMessage> request,SocksConnection *parent = nullptr);

    //pure-virtual from SocksState
    virtual void handleIncomingBytes(QByteArray& bytes) override;

    //virtual from SocksState
    virtual void handleSetAsNewState() override;
    
private slots:
    void handleSocketConnected();
    void handleSocketError(QAbstractSocket::SocketError);
    void handleDomainLookupResult(const QHostInfo& info);

private:
    void handleIP();
    void handleDomain();

    QSharedPointer<Socks5RequestMessage> _request;
    QTcpSocket *_socket;
    int _dnsLookupID;    
};

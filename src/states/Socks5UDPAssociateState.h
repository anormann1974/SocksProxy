#pragma once

#include "SocksState.h"
#include "protocol/Socks5RequestMessage.h"

#include <QSharedPointer>
#include <QHostInfo>


class Socks5UDPAssociateState : public SocksState
{
    Q_OBJECT
public:
    explicit Socks5UDPAssociateState(QSharedPointer<Socks5RequestMessage> request,
                                     SocksConnection *parent = nullptr);

    //pure-virtual from SocksState
    virtual void handleIncomingBytes(QByteArray& bytes) override;

    //virtual from SocksState
    virtual void handleSetAsNewState() override;
    
private slots:
    void handleDomainLookupResult(const QHostInfo& info);

private:
    void handleIP();
    void handleDomain();

    QSharedPointer<Socks5RequestMessage> _request;
    int _dnsLookupID;    
};

#pragma once

#include "SocksState.h"
#include "protocol/Socks5GreetingMessage.h"

#include <QSharedPointer>


class Socks5AuthState : public SocksState
{
    Q_OBJECT
public:
    explicit Socks5AuthState(QSharedPointer<Socks5GreetingMessage> clientGreeting, SocksConnection *parent = nullptr);

    //pure-virtual from SocksState
    virtual void handleIncomingBytes(QByteArray &bytes) override;

    //virtual from SocksState
    virtual void handleSetAsNewState() override;
    
private:
    QSharedPointer<Socks5GreetingMessage> _clientGreeting;
    
};

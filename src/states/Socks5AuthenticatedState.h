#pragma once

#include "SocksState.h"
#include "SocksConnection.h"
#include "protocol/Socks5RequestMessage.h"

#include <QTimer>
#include <QHostInfo>


class Socks5AuthenticatedState : public SocksState
{
    Q_OBJECT
public:
    explicit Socks5AuthenticatedState(SocksConnection *parent = nullptr);

    //pure-virtual from SocksState
    virtual void handleIncomingBytes(QByteArray &bytes) override;
    
private slots:
    void handleTimeout();

private:
    QTimer *_timeoutTimer;
};

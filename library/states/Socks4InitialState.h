#pragma once

#include "SocksState.h"


class Socks4InitialState : public SocksState
{
    Q_OBJECT
public:
    explicit Socks4InitialState(SocksConnection *parent = nullptr);

    //pure-virtual from SocksState
    virtual void handleIncomingBytes(QByteArray &bytes) override;
};

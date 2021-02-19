#pragma once

#include "SocksState.h"

#include <QPointer>
#include <QIODevice>


class Socks4ConnectedState : public SocksState
{
    Q_OBJECT
public:
    explicit Socks4ConnectedState(QIODevice *socket, SocksConnection *parent = nullptr);

    //pure-virtual from SocksState
    virtual void handleIncomingBytes(QByteArray &bytes) override;

    //virtual from SocksState
    virtual void handleSetAsNewState() override;
    
private slots:
    void handleRemoteReadyRead();
    void handleRemoteDisconnect();

private:
    QPointer<QIODevice> _socket;    
};

#pragma once

#include "SocksState.h"

#include <QTcpSocket>
#include <QPointer>
#include <QIODevice>


class Socks5ConnectedState : public SocksState
{
    Q_OBJECT
public:
    explicit Socks5ConnectedState(QIODevice *remoteSocket, SocksConnection *parent = nullptr);

    //pure-virtual from SocksState
    virtual void handleIncomingBytes(QByteArray& bytes) override;

    //virtual from SocksState
    virtual void handleSetAsNewState() override;

private:
    QPointer<QIODevice> _socket;
    bool _shutdown;
    
private slots:
    void handleRemoteReadyRead();
    void handleRemoteDisconnect();
    
};

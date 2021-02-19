#pragma once

#include "SocksState.h"

#include <QByteArray>
#include <QTimer>


class InitialState : public SocksState
{
    Q_OBJECT
public:
    explicit InitialState(SocksConnection *parent = nullptr);

    //pure-virtual from SocksState
    virtual void handleIncomingBytes(QByteArray &bytes) override;
    
private slots:
    void handleTimeout();

private:
    QTimer *_timeoutTimer;

};

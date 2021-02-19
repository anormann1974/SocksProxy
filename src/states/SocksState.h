#pragma once

#include <QObject>

#include "SocksConnection.h"


class SocksState : public QObject
{
    Q_OBJECT
public:
    explicit SocksState(SocksConnection *parent = nullptr);
    virtual ~SocksState();

    //Called when SocksConnection sets us as the new state OR when new incoming bytes come
    virtual void handleIncomingBytes(QByteArray &bytes) = 0;

    //Called when SocksConnection sets us as the new state
    virtual void handleSetAsNewState();
    
protected:
    SocksConnection * _parent;    
};

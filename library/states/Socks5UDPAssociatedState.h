#pragma once

#include "SocksState.h"

#include <QUdpSocket>
#include <QPointer>
#include <QHostAddress>
#include <QByteArray>


class Socks5UDPAssociatedState : public SocksState
{
    Q_OBJECT
public:
    explicit Socks5UDPAssociatedState(QUdpSocket *clientFacing,
                                      QUdpSocket *remoteFacing,
                                      SocksConnection *parent = nullptr);

    //pure-virtual from SocksState
    virtual void handleIncomingBytes(QByteArray& bytes) override;

    //virtual from SocksState
    virtual void handleSetAsNewState() override;
    
private slots:
    void handleRemoteReadyRead();
    void handleClientReadyRead();
    void handleRemoteDatagram(QHostAddress senderHost, quint16 senderPort, QByteArray);
    void handleClientDatagram(QHostAddress senderHost, quint16 senderPort, QByteArray);

private:
    bool readDatagram(QUdpSocket *socket, QHostAddress *senderHost , quint16 *senderPort, QByteArray *bytes);

    QPointer<QUdpSocket> _clientFacing;
    QPointer<QUdpSocket> _remoteFacing;
    quint16 _lastClientUDPPort;    
};

#pragma once

#include <QObject>
#include <QPointer>
#include <QAbstractSocket>
#include <QTcpSocket>
#include <QByteArray>
#include <QSharedPointer>
#include <QIODevice>

#include "protocol/SocksProtocolMessage.h"


class SocksState;

class SocksConnection : public QObject
{
    Q_OBJECT
public:
    explicit SocksConnection(QAbstractSocket *socket, QObject *parent = nullptr);
    virtual ~SocksConnection();

    QPointer<SocksState> connectionState();

    SocksProtocolMessage::SocksVersion socksVersion() const;
    void setSocksVersion(SocksProtocolMessage::SocksVersion);
    bool socksVersionSet() const;

    bool sendMessage(QSharedPointer<SocksProtocolMessage> msg, QString *error = nullptr);

    QHostAddress myBoundAddress() const;
    QHostAddress peerAddress() const;
       
public slots:
    void sendData(const QByteArray& toSend);
    void setState(SocksState *state);
    void close();

protected slots:
    void handleIncomingBytes(QByteArray &bytes);

private slots:
    void onReadyRead();

private:
    QPointer<SocksState> _connectionState;
    QPointer<QAbstractSocket> _rawSocket;
    QPointer<QIODevice> _socket;
    QByteArray _recvBuffer;

    bool _socksVersionSet;
    SocksProtocolMessage::SocksVersion _socksVersion;
    
};

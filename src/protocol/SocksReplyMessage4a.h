#pragma once

#include <QtGlobal>
#include <QHostAddress>
#include <QByteArray>

#include "SocksProtocolMessage.h"


class Socks5ReplyMessage4a : public SocksProtocolMessage
{
public:
    enum ReplyStatus4a
    {
        Success,
        RejectedOrFailed,
        IdentdFailed,
        IdentdUnauthorized
    };

public:
    Socks5ReplyMessage4a(ReplyStatus4a status,QHostAddress address, quint16 port);

    //pure-virtual from SocksProtocolMessage
    virtual ParseResult parse(QByteArray& bytes,QString * error = nullptr) override;

    //pure-virtual from SocksProtocolMessage
    virtual bool toBytes(QByteArray * output,QString * error=nullptr) override;

    //pure-virtual from SocksProtocolMessage
    virtual qint64 minimumMessageLength() const override;

    static bool replyStatus4a2Byte(ReplyStatus4a status, quint8 * output);

private:
    ReplyStatus4a _status;
    QHostAddress _address;
    quint16 _port;
};

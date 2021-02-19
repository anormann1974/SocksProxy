#pragma once

#include <QString>
#include <QList>

#include "SocksProtocolMessage.h"


class Socks5GreetingMessage : public SocksProtocolMessage
{
public:
    //pure-virtual from SocksProtocolMessage
    virtual ParseResult parse(QByteArray &bytes,QString *error = nullptr) override;

    //pure-virtual from SocksProtocolMessage
    virtual bool toBytes(QByteArray * output, QString *error = nullptr) override;

    //pure-virtual from SocksProtocolMessage
    virtual qint64 minimumMessageLength() const override;

    QList<quint8> authMethods() const;

private:
    QList<quint8> _authMethods;
};

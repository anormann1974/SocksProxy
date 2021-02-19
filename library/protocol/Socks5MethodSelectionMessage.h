#pragma once

#include "SocksProtocolMessage.h"


class Socks5MethodSelectionMessage : public SocksProtocolMessage
{
public:
    Socks5MethodSelectionMessage(quint8 authMethod);

    //pure-virtual from SocksProtocolMessage
    virtual ParseResult parse(QByteArray &bytes,QString *error = nullptr) override;

    //pure-virtual from SocksProtocolMessage
    virtual bool toBytes(QByteArray *output, QString *error) override;

    //pure-virtual from SocksProtocolMessage
    virtual qint64 minimumMessageLength() const override;

private:
    quint8 _authMethod;
};

#pragma once

#include "SocksProtocolMessage.h"

#include <QHostAddress>


class Socks5ReplyMessage : public SocksProtocolMessage
{
public:
    enum Socks5ReplyStatus
    {
        Success,
        GeneralError,
        DisallowedByRules,
        NetworkUnreachable,
        HostUnreachable,
        ConnectionRefused,
        TTLExpired,
        CommandNotSupported,
        AddressTypeNotSupported
    };

public:
    Socks5ReplyMessage(Socks5ReplyStatus replyStatus,
                      QHostAddress myBindAddress,
                      quint16 myBindPort);

    //pure-virtual from SocksProtocolMessage
    virtual ParseResult parse(QByteArray& bytes,QString *error = nullptr) override;

    //pure-virtual from SocksProtocolMessage
    virtual bool toBytes(QByteArray * output,QString *error = nullptr) override;

    //pure-virtual from SocksProtocolMessage
    virtual qint64 minimumMessageLength() const override;

    static bool socks5ReplyStatus2Byte(Socks5ReplyStatus status, quint8 * output);
    static bool byte2socks5ReplyStatus(quint8 byte, Socks5ReplyStatus * output);

private:
    Socks5ReplyStatus _replyStatus;
    QHostAddress _myBindAddress;
    quint16 _myBindPort;

};

#pragma once

#include <QtGlobal>
#include <QByteArray>


class SocksProtocolMessage
{
public:
    enum SocksVersion
    {
        SOCKS5,
        SOCKS4
    };

    enum AddressType
    {
        IPv4 = 0x01,
        IPv6 = 0x03,
        DomainName = 0x04
    };

    enum ParseResult
    {
        Success,
        Failure,
        NotEnoughData
    };

public:
    virtual ~SocksProtocolMessage();

    virtual ParseResult parse(QByteArray &bytes, QString *error = nullptr) = 0;

    virtual bool toBytes(QByteArray *output, QString *error = nullptr) = 0;

    virtual qint64 minimumMessageLength() const = 0;

    static bool byte2SocksVersion(quint8 byte, SocksVersion *output);
    static bool socksVersion2Byte(SocksVersion input, quint8 *output);


    static bool byte2AddressType(quint8 byte, AddressType *output);
    static bool addressType2Byte(AddressType type, quint8 *output);
};

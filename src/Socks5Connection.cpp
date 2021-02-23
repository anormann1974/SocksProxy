#include "Socks5Connection.h"
#include <QLoggingCategory>
#include <QtEndian>
#include <QHostAddress>
#include <QHostInfo>
#include <QDataStream>
#include <QTcpSocket>

#include <array>


namespace {

enum AuthMethod : uint8_t {
    NoAuth = 0,
    GssApi = 1,
    Username = 2,
    Invalid = 0xff
};

static QLoggingCategory lc("Socks5Connection");

static AuthMethod checkAuthMethod(const QByteArray &bytes)
{
    if (bytes.size() != 3)
        return Invalid;

    if (bytes[0] != 5)
        return Invalid;

    //qCDebug(lc) << __func__ << bytes.toHex(' ');

    const auto n = bytes[1];

    for (uint8_t i=0; i<n; ++i)
    {
        if (const auto method = bytes[2+i]; method == NoAuth)
        {
            return NoAuth;
        }
        else if (method == Username)
        {
            // TODO: Username and password authentification
        }
    }

    return Invalid;
}

static QByteArray authReplyMessage(AuthMethod method)
{
    QByteArray result;
    QDataStream out(&result, QIODevice::WriteOnly);

    out << uint8_t(5) << method;

    return result;
}

static QByteArray connectionReplyMessage(SocksConnection::ErrorCode status, const QHostAddress &host, uint16_t port)
{
    QByteArray result;
    QDataStream out(&result, QIODevice::WriteOnly);

    out << uint8_t(5)
        << uint8_t(status)
        << uint8_t(0);

    if (host.protocol() == QAbstractSocket::IPv4Protocol)
    {
        out << uint8_t(1) << host.toIPv4Address();
    }
    else if (host.protocol() == QAbstractSocket::IPv6Protocol)
    {
        out << uint8_t(4);
        out.writeRawData((const char*)host.toIPv6Address().c, 16);
    }
    else
    {
        // TODO: Error handling
    }

    out << port;

    return result;
}

}

Socks5Connection::Socks5Connection(QAbstractSocket *socket, const QByteArray &bytes, QObject *parent)
    : SocksConnection(socket, parent)
{
    handleIncomingBytes(bytes);
}

Socks5Connection::~Socks5Connection()
{
}

void Socks5Connection::handleIncomingBytes(const QByteArray &bytes)
{
    //qCDebug(lc) << __func__ << bytes.toHex(' ');

    switch (_state)
    {
    case Connected:
        {
            const auto am = checkAuthMethod(bytes);
            //qCDebug(lc) << "Auth method selected:" << am;

            if (am == NoAuth)
                _state = Authed;
            else if (am == Username)
                _state = NeedAuth;
            else if (am == Invalid)
            {
                deleteLater();
                return;
            }

            // Send auth response
            {
                const auto msg = authReplyMessage(am);
                _clientSocket->write(msg);
            }
        }
        break;

    case NeedAuth:
        // TODO: username/password authentification
        _clientSocket->close();
        break;

    case Authed:
        if (connectSocksTarget(bytes) == Success)
        {
            _state = Tunnel;
        }
        else
        {
            const auto msg = connectionReplyMessage(CommandNotSupported, QHostAddress::Any, 0);
            _clientSocket->write(msg);
        }
        break;

    case Tunnel:
        if (const auto written = _remoteSocket->write(bytes); written != bytes.size())
        {
            qCWarning(lc) << __func__ << "Wrote" << written << "bytes but should have written" << bytes.size() << "bytes." << _remoteSocket->errorString();
        }
        break;
    }
}

SocksConnection::ErrorCode Socks5Connection::connectSocksTarget(const QByteArray &bytes)
{
    if (bytes.size() < 5)
        return GeneralFailure;

    if (bytes[0] != 5)
        return GeneralFailure;

    // TODO: More connection commands
    if (bytes[1] != 1)
        return CommandNotSupported;

    if (bytes[2] != 0)
        return GeneralFailure;

    const auto n = bytes.size();
    const auto port = qFromBigEndian<uint16_t>(bytes.data() + n - 2);

    switch (bytes[3])
    {
    case 4:
        // IPv6
        {
            Q_IPV6ADDR ip6Addr;
            std::memcpy(ip6Addr.c, bytes.constData() + 4, 16);

            connectToHost(QHostAddress(ip6Addr), port);
        }
        break;

    case 1:
        // IPv4
        {
            uint32_t ip4Addr;
            std::memcpy(&ip4Addr, bytes.constData() + 4, 4);

            connectToHost(QHostAddress(ip4Addr), port);
        }
        break;

    case 3:
        // DNS name
        {
            const auto len = bytes[4];
            const auto minLen = 4 + 2 + len + 1;

            if (bytes.size() < minLen)
                return GeneralFailure;

            const auto hostName = QString::fromLatin1(bytes.data() + 5, len);
            QHostInfo::lookupHost(hostName, this, [this, port](const QHostInfo &host) {
                if (host.error() != QHostInfo::NoError)
                {
                    qCWarning(lc).nospace() << "Host lookup failed for host " << host.hostName() << ". Reason: " << host.errorString();
                    return;
                }

                connectToHost(host.addresses().front(), port);
            });
        }
        break;
    }

    return Success;
}

void Socks5Connection::handleRemoteConnected()
{
    const auto local = _remoteSocket->localAddress();
    const auto port = _remoteSocket->localPort();
    const auto msg = connectionReplyMessage(Success, local, port);

    _clientSocket->write(msg);
}

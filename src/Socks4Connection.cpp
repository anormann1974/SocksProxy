#include "Socks4Connection.h"

#include <QLoggingCategory>
#include <QtEndian>
#include <QHostAddress>
#include <QHostInfo>
#include <QDataStream>
#include <QTcpSocket>

#include <array>


namespace {

static QLoggingCategory lc("Socks4Connection");

}

Socks4Connection::Socks4Connection(QAbstractSocket *socket, const QByteArray &bytes, QObject *parent)
    : SocksConnection(socket, parent)
{
    handleIncomingBytes(bytes);
}

Socks4Connection::~Socks4Connection()
{
}

void Socks4Connection::handleIncomingBytes(const QByteArray &bytes)
{
    //qCDebug(lc) << __func__ << bytes.toHex(' ');

    switch (_state)
    {
    case Connected:
        {
            QDataStream in(bytes);

            uint8_t ver, cmd;
            uint16_t port;
            uint32_t ip;
            QByteArray id;

            in >> ver >> cmd >> port >> ip >> id;

            QHostAddress host(ip);

            connectToHost(host, port);
            _state = Tunnel;

            //qCDebug(lc) << __func__ << ver << cmd << port << host.toString() << id;
        }
        break;

    case Tunnel:
        if (const auto written = _remoteSocket->write(bytes); written != bytes.size())
        {
            qCWarning(lc) << __func__ << "Wrote" << written << "bytes but should have written" << bytes.size() << "bytes." << _remoteSocket->errorString();
        }
        break;

    default:
        break;
    }
}

void Socks4Connection::handleRemoteConnected()
{
    QByteArray msg;
    QDataStream out(&msg, QIODevice::WriteOnly);

    out << uint8_t(0)
        << uint8_t(0x5a)
        << uint16_t(0)
        << uint32_t(0);

    _clientSocket->write(msg);
}

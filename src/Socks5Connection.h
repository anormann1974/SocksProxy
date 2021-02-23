#pragma once

#include "SocksConnection.h"


class QHostAddress;

class Socks5Connection final : public SocksConnection
{
    Q_OBJECT

public:
    explicit Socks5Connection(QAbstractSocket *socket, const QByteArray &bytes = {}, QObject *parent = nullptr);
    virtual ~Socks5Connection();

    // SocksConnection interface
protected:
    virtual void handleIncomingBytes(const QByteArray &bytes) override;

private:
    ErrorCode connectSocksTarget(const QByteArray &bytes);
    virtual void handleRemoteConnected() override;
};


#pragma once

#include "SocksConnection.h"


class QHostAddress;

class Socks4Connection final : public SocksConnection
{
    Q_OBJECT

public:
    explicit Socks4Connection(QAbstractSocket *socket, const QByteArray &bytes = {}, QObject *parent = nullptr);
    virtual ~Socks4Connection();

    // SocksConnection interface
protected:
    virtual void handleIncomingBytes(const QByteArray &bytes) override;

private:
    virtual void handleRemoteConnected() override;
};


#pragma once

#include <QObject>
#include <QTimer>
#include <QPointer>


class QAbstractSocket;
class QIODevice;
class QHostAddress;

class SocksConnection : public QObject
{
    Q_OBJECT
public:

    enum State {
        Connected,
        NeedAuth,
        Authed,
        Tunnel
    };

    enum ErrorCode : uint8_t {
        Success = 0,
        GeneralFailure = 1,
        NotAllowed = 2,
        NetUnreachable = 3,
        HostUnreachable = 4,
        ConnRefused = 5,
        TtlExpired = 6,
        CommandNotSupported = 7,
        AddresstypeNotSupported = 8
    };

    explicit SocksConnection(QAbstractSocket *socket, QObject *parent = nullptr);
    virtual ~SocksConnection();

public slots:
    void close();

private slots:
    void onReadyRead();
    void onRemoteReadyRead();
    void onRemoteConnected();

protected:
    virtual void handleIncomingBytes(const QByteArray &bytes) = 0;
    virtual void handleRemoteConnected() = 0;
    void connectToHost(const QHostAddress &host, quint16 port);

    QPointer<QAbstractSocket> _clientSocket;
    QPointer<QAbstractSocket> _remoteSocket;
    State _state;
    QTimer _timeout;
    QString _remoteHost;
    std::chrono::milliseconds _timeoutPeriod;
};

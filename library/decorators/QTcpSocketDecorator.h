#pragma once

#include <QHostAddress>
#include <QTcpSocket>
#include <QPointer>


class QTcpSocketDecorator : public QTcpSocket
{
    Q_OBJECT
public:
    explicit QTcpSocketDecorator(QPointer<QTcpSocket> decorated, QObject *parent = nullptr);
    explicit QTcpSocketDecorator(QObject *parent = nullptr);
    virtual ~QTcpSocketDecorator();

    //virtual from QAbstractSocket
    virtual bool atEnd() const override;
    virtual qint64 bytesAvailable() const override;
    virtual qint64 bytesToWrite() const override;
    virtual bool canReadLine() const override;
    virtual void close() override;
    virtual bool isSequential() const override;
    virtual bool waitForBytesWritten(int msecs = 30000) override;
    virtual bool waitForReadyRead(int msecs = 30000) override;

protected:
    //pure-virtual from QIODevice
    virtual qint64 readData(char *data, qint64 maxlen) override;
    virtual qint64 readLineData(char *data, qint64 maxlen) override;
    virtual qint64 writeData(const char *data, qint64 len) override;

    QPointer<QTcpSocket> decorated() const;

    //These catch signals from QAbstractSocket
    virtual void handleDecoratedConnected();
    virtual void handleDecoratedDisconnected();
    virtual void handleDecoratedError(QAbstractSocket::SocketError error);
    virtual void handleDecoratedHostFound();
    virtual void handleDecoratedProxyAuthenticationRequired(const QNetworkProxy &proxy,
                                                            QAuthenticator *authenticator);
    virtual void handleDecoratedStateChanged(QAbstractSocket::SocketState state);

    //These catch signals from QIODevice
    virtual void handleDecoratedAboutToClose();
    virtual void handleDecoratedBytesWritten(qint64 bytes);
    virtual void handleDecoreatedReadChannelFinished();
    virtual void handleDecoratedReadyRead();

    //Catch signal from QObjet
    virtual void handleDecoratedDestroyed(QObject * obj=0);

    virtual void connectToHostImplementation(const QString &hostName, quint16 port, OpenMode mode);
    virtual void disconnectFromHostImplementation();
    
private:
    void connectSignalsAndSlots();
    void updateAddressesFromDecorated();

    QPointer<QTcpSocket> _decorated;
};

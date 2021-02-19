#pragma once

#include <QIODevice>
#include <QPointer>


class QIODeviceDecorator : public QIODevice
{
    Q_OBJECT
public:
    explicit QIODeviceDecorator(QIODevice *toDecorate, QObject *parent = nullptr);
    virtual ~QIODeviceDecorator();

    //These are all virtual from QIODevice
    virtual bool atEnd() const override;
    virtual qint64 bytesAvailable() const override;
    virtual qint64 bytesToWrite() const override;
    virtual bool canReadLine() const override;
    virtual void close() override;
    virtual bool isSequential() const override;
    virtual bool open(OpenMode mode) override;
    virtual qint64 pos() const override;
    virtual bool reset() override;
    virtual bool seek(qint64 pos) override;
    virtual qint64 size() const override;
    virtual bool waitForBytesWritten(int msecs) override;
    virtual bool waitForReadyRead(int msecs) override;

protected:
    //These are all pure-virtual from QIODevice
    virtual qint64 readData(char *data, qint64 maxlen) override;
    virtual qint64 readLineData(char *data, qint64 maxlen) override;
    virtual qint64 writeData(const char *data, qint64 len) override;

    virtual void handleChildAboutToClose();
    virtual void handleChildBytesWritten(qint64);
    virtual void handleChildReadChannelFinished();
    virtual void handleChildReadyRead();
    
    QPointer<QIODevice> _toDecorate;
};

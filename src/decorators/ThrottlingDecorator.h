#pragma once

#include "QIODeviceDecorator.h"

#include <QTimer>
#include <QByteArray>
#include <QTcpSocket>
#include <QElapsedTimer>


class ThrottlingDecorator : public QIODeviceDecorator
{
    Q_OBJECT
public:
    explicit ThrottlingDecorator(QAbstractSocket *toDecorate, QObject *parent = nullptr);
    explicit ThrottlingDecorator(qreal readBytesPerSecond, qreal writeBytesPerSecond, QAbstractSocket *toDecorate, QObject *parent = nullptr);
    virtual ~ThrottlingDecorator();

    //These are all virtual from QIODeviceDecorator
    virtual bool atEnd() const override;
    virtual qint64 bytesAvailable() const override;
    virtual qint64 bytesToWrite() const override;
    virtual bool canReadLine() const override;
    virtual bool waitForBytesWritten(int msecs) override;
    virtual bool waitForReadyRead(int msecs) override;

    void setReadBytesPerSecond(qint64 maxReadBytesPerSecond);
    void setWriteBytesPerSecond(qint64 maxWriteBytesPerSecond);

protected:
    //pure-virtual from QIODevice
    virtual qint64 readData(char *data, qint64 maxlen) override;
    virtual qint64 readLineData(char *data, qint64 maxlen) override;
    virtual qint64 writeData(const char *data, qint64 len) override;
    
    //These are all virtual from QIODeviceDecorator
    virtual void handleChildAboutToClose() override;
    virtual void handleChildReadChannelFinished() override;
    virtual void handleChildReadyRead() override;

private:
    void handleBuckets();
    void handleWriteQueue();
    void handleReadQueue();
    void handleMetrics();

    void commonConstructor();


    QTimer *_bucketTimer;
    qint64 _readBucket;
    qint64 _writeBucket;
    QElapsedTimer _lastBucketTime;

    qint64 _readBytesPerSecond;
    qint64 _writeBytesPerSecond;

    QByteArray _writeQueue;
    QByteArray _readQueue;

    bool _childIsFinished;
    QAbstractSocket *_cheaterSocketReference;

    qreal _bytesReadSinceLastMetric;
    qreal _bytesWrittenSinceLastMetric;
    QElapsedTimer _lastMetricTime;
};

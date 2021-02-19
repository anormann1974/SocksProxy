#pragma once

#include <QObject>
#include <QSet>
#include <QString>
#include <QHash>
#include <QSharedPointer>

#include "SocksSession.h"


class SocksSessionManager : public QObject
{
    Q_OBJECT
public:
    explicit SocksSessionManager(QObject *parent = nullptr);
    virtual ~SocksSessionManager();

private:
    QHash<QString, QSharedPointer<SocksSession> > _sessions;
    
public slots:
    virtual void handleNewConnection(QPointer<SocksConnection> newConnection);
    
};

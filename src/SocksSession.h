#pragma once

#include <QObject>
#include <QPointer>
#include <QSet>

#include "SocksConnection.h"


/*!
 \brief A collection of SocksConnection objects that are related.

*/
class SocksSession : public QObject
{
    Q_OBJECT
public:
    explicit SocksSession(QObject *parent = nullptr);
    ~SocksSession();


    QSet<QPointer<SocksConnection> > connections() const;

private:
    QSet<QPointer<SocksConnection> > _connections;
    
public slots:
    void addConnection(QPointer<SocksConnection> conn);
    void removeConnection(QPointer<SocksConnection> conn);
    
};

uint qHash(const QPointer<SocksConnection>);

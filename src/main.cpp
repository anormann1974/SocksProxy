#include "SocksServer.h"

#include <QApplication>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QMessageBox>
#include <QTranslator>


int main(int argc, char *argv[])
{
    QApplication::setOrganizationName(QStringLiteral("André Normann"));
    QApplication::setApplicationName(QStringLiteral("SocksProxy"));
    QApplication::setApplicationVersion(QStringLiteral("0.1"));

    QApplication a(argc, argv);

    QTranslator translator;
    translator.load(QLocale(), QStringLiteral("lang"), QStringLiteral("_"), QStringLiteral("translations"));
    a.installTranslator(&translator);

    if (!QSystemTrayIcon::isSystemTrayAvailable())
    {
        QMessageBox::critical(nullptr,
                              QApplication::applicationName(),
                              QCoreApplication::tr("Could not detect system tray on this system!"));
        return 1;
    }

    QApplication::setQuitOnLastWindowClosed(false);

    QScopedPointer<QSystemTrayIcon> tray(new QSystemTrayIcon(QIcon(":/icons/app_icon.svg")));
    QScopedPointer<QMenu> trayMenu(new QMenu);

    auto exitAction = trayMenu->addAction(QCoreApplication::tr("Exit"));
    QObject::connect(exitAction, &QAction::triggered, qApp, &QApplication::quit);

    tray->setContextMenu(trayMenu.get());
    tray->setToolTip(QApplication::applicationName());
    tray->show();

    QHostAddress listenip{QHostAddress::Any};
    quint16 port{1080};

    SocksServer server(listenip, port);
    server.start();

    if (!server.isStarted())
    {
        QMessageBox::critical(nullptr,
                              QApplication::applicationName(),
                              QCoreApplication::tr("Failed to start server. Shutting down."));
        a.quit();
    }
    else
    {
        tray->showMessage(QCoreApplication::tr("Application startup"), QCoreApplication::tr("SocksProxy started successfully"));
    }

    return a.exec();

}

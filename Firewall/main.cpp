#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlComponent>
#include <QQmlContext>
#include <QTextBlock>

#include "utils.h"
#include "qlinkget.h"
#include "qnattable.h"
#include "logwatcher.h"
#include "qlinktable.h"
#include "qfiltertable.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    QNat *qnat = new QNat();
    QRule *qrule = new QRule();
    QLinkGet *qthread = new QLinkGet();
    QObject *defaultpolicy = new QObject();
    QNatTable *natTableModel = new QNatTable();
    QLinkTable *linkTableModel = new QLinkTable();
    QFilterTable *filterTableModel = new QFilterTable();

    LogWatcher *logWatcher = new LogWatcher("/var/log/kern.log", "Firewall: ");

    qRegisterMetaType<QTextBlock>("QTextBlock");

    engine.rootContext()->setContextProperty("qnat", qnat);
    engine.rootContext()->setContextProperty("qrule", qrule);
    engine.rootContext()->setContextProperty("chrDev", chrDev);
    engine.rootContext()->setContextProperty("natTableModel", natTableModel);
    engine.rootContext()->setContextProperty("linkTableModel", linkTableModel);
    engine.rootContext()->setContextProperty("filterTableModel", filterTableModel);

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    logsOutput = engine.rootObjects()[0]->findChild<QObject*>("logs");
    defaultpolicy = engine.rootObjects()[0]->findChild<QObject*>("defaultpolicy");

    QObject::connect(logWatcher, &LogWatcher::newlogs, LogsAppend);
    QObject::connect(qthread, SIGNAL(newLinks(QString)), linkTableModel, SLOT(setlinkItem(QString)));
    QObject::connect(defaultpolicy, SIGNAL(activated(int)), chrDev, SLOT(setDefaultPolicy(int)));
    QObject::connect(filterTableModel, SIGNAL(addFilter(QString)), chrDev, SLOT(addFilterItem(QString)));
    QObject::connect(filterTableModel, SIGNAL(delFilter(int)), chrDev, SLOT(delFilterItem(int)));
    QObject::connect(natTableModel, SIGNAL(addNat(QString)), chrDev, SLOT(addNatItem(QString)));
    QObject::connect(natTableModel, SIGNAL(delNat(int)), chrDev, SLOT(delNatItem(int)));
    chrDev->openDev();

    //get log from kern.log
    logWatcher->start();

    //get link table from kernel
    qthread->start();

    return app.exec();
}

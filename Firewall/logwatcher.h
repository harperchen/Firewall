#ifndef LOGWATCHER_H
#define LOGWATCHER_H

#include <QObject>
#include <QString>
#include <QDebug>
#include "utils.h"

class LogWatcher: public QObject
{
    Q_OBJECT
public:
    LogWatcher();
    LogWatcher(QString file_path, QString tag = "");

    void start();
private:

    void readLogs();
    void get_event();

    QString logs;
    QString tag;
    QString watchedfile;

    int fd;
    int wd;   /* watch descriptor */
    int fileIndex;

signals:
    void newlogs(QString logs);
};

#endif // LOGWATCHER_H

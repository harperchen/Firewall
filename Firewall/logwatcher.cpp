#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/inotify.h>

#include <thread>
#include <QFile>

#include "logwatcher.h"

#define BUFF_SIZE ((sizeof(struct inotify_event) + FILENAME_MAX) * 1024)

LogWatcher::LogWatcher(QString file_path, QString tag)
{
    watchedfile = file_path;

    QFile file(watchedfile);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    fd = inotify_init();
    if (fd < 0) {
        printf("ERROR");
    }

    wd = inotify_add_watch(fd, file_path.toStdString().c_str(), IN_ALL_EVENTS);
    if (wd < 0) {
        printf("ERROR");
    }
    this->tag = tag;
    fileIndex = file.size();
    file.close();
}

LogWatcher::LogWatcher()
{
    const char* file_path = "/var/log/kern.log";
    watchedfile = file_path;

    QFile file(watchedfile);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    fd = inotify_init();
    if (fd < 0) {
        printf("ERROR");
    }

    wd = inotify_add_watch(fd, file_path, IN_ALL_EVENTS);
    if (wd < 0) {
        printf("ERROR");
    }
    fileIndex = file.size();
    file.close();
}


void LogWatcher::readLogs()
{
    QFile file(watchedfile);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    file.seek(fileIndex);

    QTextStream in(&file);
    QString line = in.readLine();
    QString displayString;
    displayString.clear();

    while (!line.isNull()) {
        if(line.contains(this->tag)){
            int index = line.indexOf("Firewall: ");
            line = line.mid(index);
            qDebug() << line;
            if(line == "Firewall: "){
                qDebug("test");
                line = "";
            }
            line += '\n';
            line.replace('#', " ");
            displayString.append(line);
        }
        line = in.readLine();
    }
    fileIndex = file.size();
    file.close();
    if(displayString.size()){
        emit newlogs(displayString);
    }
}


void LogWatcher::get_event()
{
    ssize_t len, i = 0;
    char buff[BUFF_SIZE] = {0};

    len = read(fd, buff, BUFF_SIZE);
    while(i < len){
        struct inotify_event *pevent = (struct inotify_event *)&buff[i];

        if(pevent->mask & IN_MODIFY){
            qDebug() << "modify";
            readLogs();
        }
        i += sizeof(struct inotify_event) + pevent->len;
    }
}

void LogWatcher::start(){
    std::thread t([this](){
        qDebug() << "Starting Loop;\n";
        while(1){
            get_event();
        }
    });
    t.detach();
}


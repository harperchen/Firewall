#ifndef CHRDEV_H
#define CHRDEV_H
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include <QString>
#include <QObject>
#include <QDebug>
#include "kernel/output.h"

class ChrDev : public QObject
{
    Q_OBJECT

public:
    ChrDev();
    ~ChrDev();

    void openDev();

    Q_INVOKABLE void closeDev();
    QString copyLinks();

public slots:
    void addFilterItem(QString filterItem);
    void delFilterItem(int row);

    void addNatItem(QString natItem);
    void delNatItem(int row);

    void setDefaultPolicy(int index);
private:
    int fd;
    char *msg;
};

#endif // CHRDEV_H

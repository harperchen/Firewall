#ifndef QLINKGET_H
#define QLINKGET_H

#include <QThread>
#include <QDebug>
#include "utils.h"

class QLinkGet : public QThread
{
    Q_OBJECT
public:
    QLinkGet();
    void run();
signals:
    void newLinks(QString str);
};

#endif // QLINKGET_H

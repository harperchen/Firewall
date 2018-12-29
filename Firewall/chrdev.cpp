#include "chrdev.h"

ChrDev::ChrDev()
{
    msg = new char[1024];
}

ChrDev::~ChrDev(){
    delete []msg;
}

void ChrDev::openDev()
{
    fd = open("/dev/chrdev", O_RDWR | O_CREAT, 0644);
    if(fd == -1){
        qDebug() << "Fail";
    }
    else{
        qDebug() << "Successfully";
    }
}

void ChrDev::closeDev()
{
    close(fd);
    qDebug() << "close /dev/chrdev successfully!\n";
}

void ChrDev::addFilterItem(QString filterItem)
{
    memset(msg, 0, 1024);
    qDebug() << "add filter item" << filterItem;
    qDebug() << "filter string size" << filterItem.size();
    *(int *)msg = filterItem.size();
    memcpy(msg + 4, filterItem.toLatin1().data(), filterItem.size());
    ioctl(fd, ADD_FILTER_FROM_USER, msg);
}

void ChrDev::delFilterItem(int row)
{
    qDebug() << "del filter item" << row;
    ioctl(fd, DEL_FILTER_FROM_USER, row);
}

void ChrDev::addNatItem(QString natItem)
{
    memset(msg, 0, 1024);
    qDebug() << "add nat item" << natItem;
    qDebug() << "nat string size" << natItem.size();
    *(int *)msg = natItem.size();
    memcpy(msg + 4, natItem.toLatin1().data(), natItem.size());
    ioctl(fd, ADD_NAT_FROM_USER, msg);
}

void ChrDev::delNatItem(int row)
{
    qDebug() << "del nat item" << row;
    ioctl(fd, DEL_NAT_FROM_USER, row);
}

void ChrDev::setDefaultPolicy(int index)
{
    qDebug() << "current index is "<< index;
    ioctl(fd, SET_DEFAULT_POLICY, 1 - index);
}

QString ChrDev::copyLinks()
{
    memset(msg, 0, 1024);
    ioctl(fd, COPY_LINK_TO_USER, msg);
    QString str = QString(QLatin1String(msg));
    return str;
}

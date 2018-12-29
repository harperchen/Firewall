#include "utils.h"

ChrDev *chrDev = new ChrDev();
QObject *logsOutput = new QObject();

void LogsAppend(QString str)
{
    QMetaObject::invokeMethod(logsOutput, "append", Qt::DirectConnection,
                              Q_ARG(QVariant, QVariant(str)));
}


bool AscendingOrder_sip(const Link &s1, const Link &s2){
    return s1.source_ip.toLower() < s2.source_ip.toLower();
}

bool DescendingOrder_sip(const Link &s1, const Link &s2){
    return s1.source_ip.toLower() > s2.source_ip.toLower();
}

bool AscendingOrder_dip(const Link &s1, const Link &s2){
    return s1.dest_ip.toLower() < s2.dest_ip.toLower();
}

bool DescendingOrder_dip(const Link &s1, const Link &s2){
    return s1.dest_ip.toLower() > s2.dest_ip.toLower();
}

bool AscendingOrder_prot(const Link &s1, const Link &s2){
    return s1.protocol.toLower() < s2.protocol.toLower();
}

bool DescendingOrder_prot(const Link &s1, const Link &s2){
    return s1.protocol.toLower() > s2.protocol.toLower();
}

bool AscendingOrder_time(const Link &s1, const Link &s2){
    return s1.timeout.toLower() < s2.timeout.toLower();
}

bool DescendingOrder_time(const Link &s1, const Link &s2){
    return s1.timeout.toLower() > s2.timeout.toLower();
}

#ifndef UTILS_H
#define UTILS_H

#include "chrdev.h"
#include "rule.h"

extern ChrDev *chrDev;
extern QObject *logsOutput;

extern void LogsAppend(QString str);

extern bool AscendingOrder_prot(const Link &s1, const Link &s2);
extern bool DescendingOrder_prot(const Link &s1, const Link &s2);

extern bool AscendingOrder_sip(const Link &s1, const Link &s2);
extern bool DescendingOrder_sip(const Link &s1, const Link &s2);

extern bool AscendingOrder_dip(const Link &s1, const Link &s2);
extern bool DescendingOrder_dip(const Link &s1, const Link &s2);

extern bool AscendingOrder_time(const Link &s1, const Link &s2);
extern bool DescendingOrder_time(const Link &s1, const Link &s2);

#endif // UTILS_H

#ifndef UTILS_H_
#define UTILS_H_

#include <linux/timer.h>
#include <linux/timex.h>
#include <linux/rtc.h>
#include <linux/types.h>
#include <linux/netfilter.h>

#define _LOG_TAG_ "Firewall: "

#define firewall_printk(switches, fmt, ...) \
    ((switches) ? (printk(_LOG_TAG_ pr_fmt(fmt), ##__VA_ARGS__)) : (printk(pr_fmt(fmt), ##__VA_ARGS__)))

extern unsigned int defaultAction;

typedef struct myIP{
	__be32 from;
	__be32 to;
}myIP;

typedef struct myPort{
	__be16 from;
	__be16 to;
}myPort;

unsigned int addrtohex(char* str);
char* hextoaddr(unsigned int addr);
unsigned int addr_change(unsigned int addr);

void right_time(void);

#endif

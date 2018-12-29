#ifndef FILTER_H_
#define FILTER_H_

#include <linux/in.h>
#include <linux/rtc.h>
#include <linux/icmp.h>
#include <linux/kernel.h>

#include "nat.h"
#include "utils.h"
#include "hashTable.h"
#include "lineTable.h"

typedef enum TCP_STATUS{
	SYN_SENT,
	ESTABLISHED,
	FIN_WAIT_1,
	FIN_WAIT_2,
	TIME_WAIT_1
}TCP_STATUS;

struct filterItem{
	myIP sip;
	myPort sport;
	myIP dip;
	myPort dport;

	__u8 protocol;
	unsigned int chain;

	bool action;
	bool log;
	int index;
};

struct linkItem{
	__be32 saddr;
	__be32 daddr;
	__be16 sport;
	__be16 dport;
	__u8 protocol;
	__u8 type;
	__be32 seq;
	__be32 ack_seq;
	TCP_STATUS tcp_status; 	
	
	unsigned int chain;
	bool log;
	long timeout;
	int index;
};

extern HashTable *LinkTable;
extern LineTable *FilterTable;

void add_filterItem(char *str);
void del_filterItem(int row);

bool search_filter_item(void *a, void *b, unsigned int hooknum);
bool compare_filter_item(void *a, void *b);
void free_filter_item(void *item);

char *link_to_str(struct linkItem *item, bool flag);
void free_link_item(void *item);
char *item_to_str(void *a);
bool link_item_update(void *a);
bool link_item_clear(void *a, int index);

#endif
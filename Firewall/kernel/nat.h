#ifndef NAT_H_
#define NAT_H_

#include <linux/in.h>
#include <linux/kernel.h>

#include "utils.h"
#include "lineTable.h"

typedef enum NAT_TYPE{
	STATIC,
	DYNAMIC,
	PAT
}NAT_TYPE;

struct natSearch{
    __be32 addr;
    __be16 port;
    __u8 protocol;
};

struct natItem{
	myIP before_ip;
	myPort before_port;
	myIP after_ip;
	myPort after_port;
	NAT_TYPE type;
	int index;
};

struct mapItem{
    __be32 before_ip;
    __be16 before_port;
    __be32 after_ip;
    __be16 after_port;
	NAT_TYPE type;
	int index;
};

extern LineTable *MapTable;
extern LineTable *NatTable;

void add_natItem(char *str);
void del_natItem(int row);

bool compare_nat_item(void *a, void *b);
bool search_nat_item(void *a, void *b, unsigned int hooknum);
void free_nat_item(void *item);

bool compare_map_item(void *a, void *b);
bool search_map_item(void *a, void *b, unsigned int hooknum);
void free_map_item(void *item);
bool map_item_clear(void *a, int index);

#endif
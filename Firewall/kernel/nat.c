#include "nat.h"

LineTable *MapTable = NULL;
LineTable *NatTable = NULL;

static int global_index = 0;

bool compare_nat_item(void *a, void *b){
    struct natItem *item_a, *item_b;
    item_a = (struct natItem *)a;
    item_b = (struct natItem *)b;
    return ((item_a->before_ip.from == item_b->before_ip.from) && (item_a->before_ip.to == item_b->before_ip.to)
         && (item_a->after_ip.from == item_b->after_ip.from) && (item_a->after_ip.to == item_b->after_ip.to)
         && (item_a->before_port.from == item_b->before_port.from) && (item_a->before_port.to == item_b->before_port.to)
         && (item_a->after_port.from == item_b->after_port.from) && (item_a->after_port.to == item_b->after_port.to)
         && (item_a->type == item_b->type));
}

bool compare_map_item(void *a, void *b){
    struct mapItem *item_a, *item_b;
    item_a = (struct mapItem *)a;
    item_b = (struct mapItem *)b;
    return (item_a->before_ip == item_b->before_ip)
    && (item_a->before_port == item_b->before_port)
    && (item_a->after_ip == item_b->after_ip)
    && (item_a->after_port == item_b->after_port)
    && (item_a->type == item_b->type);
}

void free_nat_item(void *item){
    if(item){
        printk("kfree nat item %lx\n", (long unsigned int)item);
        kfree(item);
    }
}

void free_map_item(void *item){
    if(item){
        printk("kfree map item %lx\n", (long unsigned int)item);
        kfree(item);
    }
}

bool search_nat_item(void *a, void *b, unsigned int hooknum){
    struct natSearch *packet = (struct natSearch *)a;
    struct natItem *item = (struct natItem *)b;
    
    if((packet->addr < item->before_ip.from) || (packet->addr > item->before_ip.to)){
        return false;
    }
    if(packet->protocol != IPPROTO_ICMP){
        if((packet->port < item->before_port.from) || (packet->port > item->before_port.to)){
            return false; 
        }
    }
    return true;
}

bool search_map_item(void *a, void *b, unsigned int hooknum){
    struct natSearch *packet = (struct natSearch *)a;
    struct mapItem *item = (struct mapItem *)b;

    if((packet->addr != item->before_ip) && (packet->addr != item->after_ip)){
        return false;
    }
    if(item->type != PAT){
        return true;
    }
    if((packet->addr == item->before_ip) && packet->protocol != IPPROTO_ICMP){
        if(packet->port != item->before_port){
            return false;
        }
        return true;
    }
    if((packet->addr == item->after_ip) && (packet->protocol != IPPROTO_ICMP)){
        if(packet->port != item->after_port){
            return false;
        }
        return true;
    }
    return true;
}

void add_natItem(char *str){
    int i = 0, mask_int = 0;
    char array[3][50], *ptr = str, *qtr;
    struct natItem *item;
    char ip[20], port[10], mask[5];

    memset(array, 0, 3 * 50);
    
    for(i = 0; i < 2; i++){
        qtr = strchr(ptr, '#');
        memcpy(array[i], ptr, strlen(ptr) - strlen(qtr));
        ptr = qtr + 1;
    }
    memcpy(array[2], ptr, strlen(ptr));

    item = kmalloc(sizeof(struct natItem), GFP_KERNEL);
    memset(item, 0, sizeof(struct natItem));
    
    //before address
    ptr = array[0];
    qtr = strchr(ptr, '/');
    memset(ip, 0, 20);
    memcpy(ip, ptr, strlen(ptr) - strlen(qtr));
    ptr = qtr + 1;

    qtr = strchr(ptr, ':');
    memset(mask, 0, 5);
    memcpy(mask, ptr, strlen(ptr) - strlen(qtr));
    ptr = qtr + 1;

    memset(port, 0, 10);
    memcpy(port, ptr, strlen(ptr));

    if(strcmp(ip, "ANY") == 0){
        item->before_ip.from = 0x0;
        item->before_ip.to = 0xFFFFFFFF;
    }
    else{
        item->before_ip.from = addrtohex(ip);
    }

    if(strcmp(mask, "") != 0){
        sscanf(mask, "%d", &mask_int);
        item->before_ip.from = item->before_ip.from & (0xFFFFFFFF << (32 - mask_int));
        item->before_ip.to = item->before_ip.from | (0xFFFFFFFF >> mask_int);
    }else{
        if(strcmp(ip, "ANY") != 0){
            item->before_ip.to = item->before_ip.from;
        }
    }

    if(strcmp(port, "") != 0){
        if(strcmp(port, "ANY") == 0){
            item->before_port.from = 0;
            item->before_port.to = 0xFFFF;
        }
        else if(strchr(port, ':') == NULL){
            sscanf(port, "%hd", &item->before_port.from);
            item->before_port.to = item->before_port.from;
        }
        else{
            ptr = port;
            qtr = strchr(ptr, ':');
            sscanf(qtr + 1, "%hd", &item->before_port.to);
            memset(qtr, 0, strlen(qtr));
            sscanf(ptr, "%hd", &item->before_port.from);
        }
    }
    
    //dest address
    ptr = array[1];
    qtr = strchr(ptr, '/');
    memset(ip, 0, 20);
    memcpy(ip, ptr, strlen(ptr) - strlen(qtr));
    ptr = qtr + 1;

    qtr = strchr(ptr, ':');
    memset(mask, 0, 5);
    memcpy(mask, ptr, strlen(ptr) - strlen(qtr));
    ptr = qtr + 1;

    memset(port, 0, 10);
    memcpy(port, ptr, strlen(ptr));

    if(strcmp(ip, "ANY") == 0){
        item->after_ip.from = 0x0;
        item->after_ip.to = 0xFFFFFFFF;
    }
    else{
        item->after_ip.from = addrtohex(ip);
    }

    if(strcmp(mask, "") != 0){
        sscanf(mask, "%d", &mask_int);
        item->after_ip.from = item->after_ip.from & (0xFFFFFFFF << (32 - mask_int));
        item->after_ip.to = item->after_ip.from | (0xFFFFFFFF >> mask_int);
    }else{
        if(strcmp(ip, "ANY") != 0){
            item->after_ip.to = item->after_ip.from;
        }
    }

    if(strcmp(port, "") != 0){
        if(strcmp(port, "ANY") == 0){
            item->after_port.from = 0;
            item->after_port.to = 0xFFFF;
        }
        else if(strchr(port, ':') == NULL){
            sscanf(port, "%hd", &item->after_port.from);
            item->after_port.to = item->after_port.from;
        }
        else{
            ptr = port;
            qtr = strchr(ptr, ':');
            sscanf(qtr + 1, "%hd", &item->after_port.to);
            memset(qtr, 0, strlen(qtr));
            sscanf(ptr, "%hd", &item->after_port.from);
        }
    }

    if(strcmp(array[2], "Static Nat") == 0){
        item->type = STATIC;
    }
    else if(strcmp(array[2], "Dynamic Nat") == 0){
        item->type = DYNAMIC;
    }
    else{
        item->type = PAT;
    }


    item->index = global_index;
    global_index++;

    line_table_put(NatTable, item);
}

void del_natItem(int row){
    int index = -1;
    struct natItem *item;
    printk("del nat table %d\n", row);
    item = NatTable->table[row];
    index = item->index;
    line_table_rm(NatTable, row);
    line_table_clear(MapTable, &map_item_clear, index);
}

bool map_item_clear(void *a, int index){
    struct mapItem *item = (struct mapItem *)a;
    if(item->index == index){
        return true;
    }
    return false;
}
#include "filter.h"

#define IPPROTO_ANY 3

#define TCP_TIMEOUT 20
#define UDP_TIMEOUT 10
#define ICMP_TIMEOUT 5

HashTable *LinkTable = NULL;
LineTable *FilterTable = NULL;

static int global_index = 0;

bool compare_filter_item(void *a, void *b){
    struct filterItem *item_a, *item_b;
    item_a = (struct filterItem *)a;
    item_b = (struct filterItem *)b;
    return ((item_a->sip.from == item_b->sip.from) && (item_a->sip.to == item_b->sip.to)
        && (item_a->dip.from == item_b->dip.from) && (item_a->dip.to == item_b->dip.to)
        && (item_a->sport.from == item_b->sport.from) && (item_a->sport.to == item_b->sport.to) 
        && (item_a->dport.from == item_b->dport.from) && (item_a->dport.to == item_b->dport.to)
        && (item_a->chain == item_b->chain) && (item_a->protocol == item_b->protocol)
        && (item_a->action == item_b->action) && (item_a->log == item_b->log));
}

void free_filter_item(void *item){
    if(item){
        printk("kfree filter item %lx\n", (long unsigned int)item);
        kfree(item);
    }
}


void free_link_item(void *item){
    if(item){
        printk("kfree link item %lx\n", (long unsigned int)item);
        kfree(item);
    }
}

bool search_filter_item(void *a, void *b, unsigned int hooknum){
    struct linkItem *packet = (struct linkItem *)a;
    struct filterItem *item = (struct filterItem *)b;

    if(hooknum != item->chain){
        return false;
    }
    if((packet->protocol != item->protocol) && (item->protocol != IPPROTO_ANY)){
        return false;
    }
    if((packet->saddr < item->sip.from) || (packet->saddr > item->sip.to)){
        return false;
    }
    if((packet->daddr < item->dip.from) || (packet->daddr > item->dip.to)){
        return false;
    }
    
    if((packet->protocol == IPPROTO_TCP) || (packet->protocol == IPPROTO_UDP)){
        //TCP和UDP检查端口
        if((packet->sport < item->sport.from) || (packet->sport > item->sport.to)){
            return false;
        }
        if((packet->dport < item->dport.from) || (packet->dport > item->dport.to)){
            return false;
        }
    }
    return true;
}

void add_filterItem(char *str){
    int i = 0, mask_int = 0;
    struct filterItem *item;
    char array[6][50], *ptr = str, *qtr = str;
    char ip[20], port[10], mask[5];
    memset(array, 0, 6 * 50);
     
    for(i = 0; i < 5; i++){
        qtr = strchr(ptr, '#');
        memcpy(array[i], ptr, strlen(ptr) - strlen(qtr));
        ptr = qtr + 1;
    }
    strcpy(array[5], ptr);
    

    item = kmalloc(sizeof(struct filterItem), GFP_KERNEL);
    memset(item, 0, sizeof(struct filterItem));
    
    //source address
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
        item->sip.from = 0x0;
        item->sip.to = 0xFFFFFFFF;
    }
    else{
        item->sip.from = addrtohex(ip);
    }

    if(strcmp(mask, "") != 0){
        sscanf(mask, "%d", &mask_int);
        item->sip.from = item->sip.from & (0xFFFFFFFF << (32 - mask_int));
        item->sip.to = item->sip.from | (0xFFFFFFFF >> mask_int);
    }else{
        if(strcmp(ip, "ANY") != 0){
            item->sip.to = item->sip.from;
        }
    }

    if(strcmp(port, "") != 0){
        if(strcmp(port, "ANY") == 0){
            item->sport.from = 0;
            item->sport.to = 0xFFFF;
        }
        else if(strchr(port, ':') == NULL){
            sscanf(port, "%hd", &item->sport.from);
            item->sport.to = item->sport.from;
        }
        else{
            ptr = port;
            qtr = strchr(ptr, ':');
            sscanf(qtr + 1, "%hd", &item->sport.to);
            memset(qtr, 0, strlen(qtr));
            sscanf(ptr, "%hd", &item->sport.from);
        }
    }

    printk("%x %x %d %d\n", item->sip.from, item->sip.to, item->sport.from, item->sport.to);

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
        item->dip.from = 0x0;
        item->dip.to = 0xFFFFFFFF;
    }
    else{
        item->dip.from = addrtohex(ip);
    }

    if(strcmp(mask, "") != 0){
        sscanf(mask, "%d", &mask_int);
        item->dip.from = item->dip.from & (0xFFFFFFFF << (32 - mask_int));
        item->dip.to = item->dip.from | (0xFFFFFFFF >> mask_int);
    }else{
        if(strcmp(ip, "ANY") != 0){
            item->dip.to = item->dip.from;
        }
    }

    if(strcmp(port, "") != 0){
        if(strcmp(port, "ANY") == 0){
            item->dport.from = 0;
            item->dport.to = 0xFFFF;
        }
        else if(strchr(port, ':') == NULL){
            sscanf(port, "%hd", &item->dport.from);
            item->dport.to = item->dport.from;
        }
        else{
            ptr = port;
            qtr = strchr(ptr, ':');
            sscanf(qtr + 1, "%hd", &item->dport.to);
            memset(qtr, 0, strlen(qtr));
            sscanf(ptr, "%hd", &item->dport.from);
        }
    }

    printk("%x %x %d %d\n", item->dip.from, item->dip.to, item->dport.from, item->dport.to);


    if(strcmp(array[2], "TCP") == 0){
        item->protocol = IPPROTO_TCP;
    }
    else if(strcmp(array[2], "UDP") == 0){
        item->protocol = IPPROTO_UDP;
    }
    else if(strcmp(array[2], "ICMP") == 0){
        item->protocol = IPPROTO_ICMP;
    }
    else if(strcmp(array[2], "ANY") == 0){
        item->protocol = IPPROTO_ANY;
    }


    if(strcmp(array[3], "INPUT") == 0){
        item->chain = NF_INET_LOCAL_IN;
        printk("Put filter item into input chain\n");
    }
    else if(strcmp(array[3], "OUTPUT") == 0){
        item->chain = NF_INET_LOCAL_OUT;
        printk("Put filter item into output chain\n");
    }
    else{
        item->chain = NF_INET_FORWARD;
        printk("Put filter item into forward chain\n");
    }

    item->action = strcmp(array[4], "Y") == 0 ? true : false;
    item->log = strcmp(array[5], "Y") == 0 ? true : false;

    printk("%d %d %d\n", item->protocol, item->action, item->log);

    item->index = global_index;
    printk("%d\n", item->index);
    global_index++;
    line_table_put(FilterTable, item);
}

void del_filterItem(int row){
    int index = -1;
    struct filterItem *item;
    printk("del filter table %d\n", row);
    item = FilterTable->table[row];
    index = item->index;
    line_table_rm(FilterTable, row);
    hash_table_clear(LinkTable, &link_item_clear, index);
}

char *link_to_str(struct linkItem *item, bool flag){
    char *ret;
    ret = (char *)kmalloc(100, GFP_ATOMIC);
    memset(ret, 0, 100);
    if(flag){
        if(item->protocol == IPPROTO_ICMP){
            sprintf(ret, "%x,%x,%d", item->saddr, item->daddr, item->protocol);
        }
        else if((item->protocol == IPPROTO_TCP) || (item->protocol == IPPROTO_UDP)){
            sprintf(ret, "%x:%d,%x:%d,%d", item->saddr, item->sport, item->daddr, item->dport, item->protocol);
        }
    }
    else{
        if(item->protocol == IPPROTO_ICMP){
            sprintf(ret, "%x,%x,%d", item->daddr, item->saddr, item->protocol);
        }
        else if((item->protocol == IPPROTO_TCP) || (item->protocol == IPPROTO_UDP)){
            sprintf(ret, "%x:%d,%x:%d,%d", item->daddr, item->dport, item->saddr, item->sport, item->protocol);
        }
    }
    return ret;
}

char *item_to_str(void *a){
    __be32 before_addr;
    __be16 before_port;
    int search_ret = -1;
    struct rtc_time tt;
    char *str, *addr, *q, *temp;
    struct linkItem *item = (struct linkItem *)a;
    struct natSearch *dnat = NULL, *snat = NULL, *nat = NULL;
    struct mapItem *map = NULL;

    dnat = kmalloc(sizeof(struct natSearch), GFP_KERNEL);
    memset(dnat, 0, sizeof(struct natSearch));

    snat = kmalloc(sizeof(struct natSearch), GFP_KERNEL);
    memset(snat, 0, sizeof(struct natSearch));

    str = kmalloc(200, GFP_KERNEL);
    memset(str, 0, 200);
    q = str;
    addr = hextoaddr(item->saddr);
    memcpy(q, addr, strlen(addr));
    q += strlen(addr);
    *q = '#';
    q++;
    kfree(addr);
    snat->addr = item->saddr;

    addr = hextoaddr(item->daddr);
    memcpy(q, addr, strlen(addr));
    q += strlen(addr);
    *q = '#';
    q++;
    kfree(addr);
    dnat->addr = item->daddr;

    temp = kmalloc(20, GFP_KERNEL);
    memset(temp, 0, 20);
    sprintf(temp, "%d#%d#", item->sport, item->dport);
    memcpy(q, temp, strlen(temp));
    q += strlen(temp);
    kfree(temp);
    snat->port = item->sport;
    dnat->port = item->dport;
    
    dnat->protocol = snat->protocol = item->protocol;
    if(item->protocol == IPPROTO_TCP){
        memcpy(q, "TCP#", 4);
        q += 4;
    }
    else if(item->protocol == IPPROTO_UDP){
        memcpy(q, "UDP#", 4);
        q += 4;
    }
    else{
        memcpy(q, "ICMP#", 5);
        q += 5;
    }
     
    if(item->type == ICMP_ECHO){
        memcpy(q, "ECHO#", 5);
        q += 5;
    }
    else if(item->type == ICMP_ECHOREPLY){
        memcpy(q, "ECHO REPLY#", 11);
        q += 11;
    }
    else{
        *q = '#';
        q++;
    }

    temp = kmalloc(50, GFP_KERNEL);
    memset(temp, 0, 50);
    sprintf(temp, "%u#%u#", item->seq, item->ack_seq);
    memcpy(q, temp, strlen(temp));
    q += strlen(temp);
    kfree(temp);

    if(item->tcp_status == SYN_SENT){
        memcpy(q, "SYN_SENT#", 9);
        q += 9;
    }
    else if(item->tcp_status == ESTABLISHED){
        memcpy(q, "ESTABLISHED#", 12);
        q += 12;
    }
    else if(item->tcp_status == FIN_WAIT_1){
        memcpy(q, "FIN_WAIT_1#", 11);
        q += 11;
    }
    else if(item->tcp_status == FIN_WAIT_2){
        memcpy(q, "FIN_WAIT_2#", 11);
        q += 11;
    }
    else if(item->tcp_status == TIME_WAIT_1){
        memcpy(q, "TIME_WAIT#", 10);
        q += 10;
    }
    else{
        *q = '#';
        q++;
    }

    rtc_time_to_tm(item->timeout, &tt);
    temp = kmalloc(30, GFP_KERNEL);
    memset(temp, 0, 30);
    sprintf(temp, "%02d:%02d:%02d", tt.tm_hour + 8, tt.tm_min, tt.tm_sec);
    memcpy(q, temp, strlen(temp));
    q += strlen(temp);
    kfree(temp);

    search_ret = line_table_search(MapTable, snat, 0);
    if(search_ret == -1){
        kfree(snat);
        search_ret = line_table_search(MapTable, dnat, 0);
        if(search_ret == -1){
            kfree(dnat);
            return str;
        }
        nat = dnat;
    }
    else{
        nat = snat;
        kfree(dnat);
    }
    *q = '#';
    q++;
    map = MapTable->table[search_ret];
	if(map->before_ip == nat->addr){
		before_addr = map->after_ip;
		before_port = map->after_port;
	}
	else{
		before_addr = map->before_ip;
		before_port = map->before_port;
	}

    addr = hextoaddr(before_addr);
    memcpy(q, addr, strlen(addr));
    q += strlen(addr);
    *q = '#';
    q++;
    kfree(addr);

    temp = kmalloc(10, GFP_KERNEL);
    memset(temp, 0, 10);
    sprintf(temp, "%d", before_port);
    memcpy(q, temp, strlen(temp));
    q += strlen(temp);
    kfree(temp);

    return str;
}


bool link_item_update(void *a){
    struct linkItem *item = (struct linkItem *)a;
    struct timespec spec = current_kernel_time();
    long timeout = spec.tv_sec - item->timeout;


    if(item->protocol == IPPROTO_TCP){
        if(timeout >= TCP_TIMEOUT){
            firewall_printk(true, "TCP...saddr %s daddr %s sport: %d dport: %d timeout\n", hextoaddr(item->saddr), hextoaddr(item->daddr), item->sport, item->dport);
            return true;
        }
    }
    else if(item->protocol == IPPROTO_UDP){
        if(timeout >= UDP_TIMEOUT){
            firewall_printk(true, "UDP...saddr %s daddr %s sport: %d dport: %d timeout\n", hextoaddr(item->saddr), hextoaddr(item->daddr), item->sport, item->dport);
            return true;
        }
    }
    else if(item->protocol == IPPROTO_ICMP){
        if(timeout >= ICMP_TIMEOUT){
            firewall_printk(true, "ICMP...saddr %s daddr %s timeout\n", hextoaddr(item->saddr), hextoaddr(item->daddr));
            return true;
        }
    }
    return false;
}

bool link_item_clear(void *a, int index){
    struct linkItem *item = (struct linkItem *)a;
    if(item->index == index){
        return true;
    }
    return false;
}
#include "utils.h"

unsigned int defaultAction = NF_ACCEPT;

unsigned int addrtohex(char* str){
    int a, b, c, d;
    char arr[4];
    sscanf(str, "%d.%d.%d.%d", &a, &b, &c, &d);
    arr[0] = d;
    arr[1] = c;
    arr[2] = b;
    arr[3] = a;
    return *(unsigned int*)arr;
}

char* hextoaddr(unsigned int addr){
    char *dest;
    int a, b, c, d;
    dest = kmalloc(20, GFP_ATOMIC);
    memset(dest, 0, 20);
    d =  addr & 0xff;
    c = (addr & 0xff00) >> 8;
    b = (addr & 0xff0000) >> 16;
    a = (addr & 0xff000000) >> 24;
    sprintf(dest, "%d.%d.%d.%d", a, b, c, d);
    return dest;
}

unsigned int addr_change(unsigned int addr){
    int a, b, c, d, ret = 0;
    a =  addr & 0xff;
    b = (addr & 0xff00) >> 8;
    c = (addr & 0xff0000) >> 16;
    d = (addr & 0xff000000) >> 24;
	ret = (a << 24) | (b << 16) | (c << 8) | d;
	return ret;
}


void right_time(void){
    struct timex txc;
    struct rtc_time tm;
    struct timeval tv;
    do_gettimeofday(&tv);
    do_gettimeofday(&(txc.time));
    rtc_time_to_tm(txc.time.tv_sec,&tm);
    printk("UTC time :%d-%d-%d %d:%d:%d\n",tm.tm_year+1900,tm.tm_mon, tm.tm_mday,tm.tm_hour,tm.tm_min,tm.tm_sec);
    printk("microsecond:%ld\n", tv.tv_usec);  //微秒
}
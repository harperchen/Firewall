#include <linux/fs.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/module.h>

#include "hook.h"
#include "output.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("harper");

#define MAX_SIZE 1024
#define MAJOR_NUM 244

static char msg[MAX_SIZE];
static int mutex = 0;

static int chr_open(struct inode *inode, struct file *file)
{
    printk("Device open\n");
    if (mutex){
        printk("busy\n");
        return -EBUSY;
    }
    mutex = 1;
    LinkTable = hash_table_new(&free_link_item);
    FilterTable = line_table_new(&compare_filter_item, &free_filter_item, &search_filter_item);
    NatTable = line_table_new(&compare_nat_item, &free_nat_item, &search_nat_item);
    MapTable = line_table_new(&compare_map_item, &free_map_item, &search_map_item);
    printk("Tables initial successfully\n");
    return 0;
}

static int chr_release(struct inode *inode, struct file *file)
{
    printk("Device released!\n");
    mutex = 0;
    line_table_delete(MapTable);
    line_table_delete(NatTable);
    line_table_delete(FilterTable);
    hash_table_delete(LinkTable);
    LinkTable = NULL;
    MapTable = NULL;
    NatTable = NULL;
    FilterTable = NULL;
    defaultAction = NF_ACCEPT;
    printk("device free\n");
    return 0;
}

static long chr_ioctl(struct file *file, unsigned int cmd, unsigned long arg){
    char *links;
    int size, row;
    switch(cmd){
        case ADD_FILTER_FROM_USER:{
            right_time();
            memset(msg, 0, MAX_SIZE);
            size = *(int *)arg;
            printk("%d\n", size);
            if(copy_from_user(msg, (char *)(arg + 4), size))
                return -EFAULT;
            firewall_printk(true, "add filter item:%s, size is %d\n", msg, size);
            add_filterItem(msg);
            break;
        } 
        case DEL_FILTER_FROM_USER:{
            row = arg;
            del_filterItem(row);
            firewall_printk(true, "delete filter item:%d\n", row);
            break;
        }
            
        case ADD_NAT_FROM_USER:{
            memset(msg, 0, MAX_SIZE);
            size = *(int *)arg;
            if(copy_from_user(msg, (char *)(arg + 4), size))
                return -EFAULT;
            add_natItem(msg);
            firewall_printk(true, "add nat item: %s, size is %d\n", msg, size);
            break;
        }
            
        case DEL_NAT_FROM_USER:{
            row = arg;
            del_natItem(row);
            firewall_printk(true, "delete nat item: %d\n", row);
            break;
        }
            
        case COPY_LINK_TO_USER:{
            links = hash_table_str(LinkTable, item_to_str);
            size = strlen(links);
            if(copy_to_user((char *)arg, links, strlen(links))){
                return -EFAULT;
            }
            memset(links, 0, size);
            kfree(links);
            break;
        }

        case SET_DEFAULT_POLICY:{
            if(arg){
                defaultAction = NF_ACCEPT;
                firewall_printk(true, "default policy is accept\n");
            }
            else{
                defaultAction = NF_DROP;
                firewall_printk(true, "default policy is drop\n");
            }
            break;
        }
            
        default:
            return -EFAULT;
             
    }
    return 0;
}

struct file_operations chr_fops = {
    .open    = chr_open,
    .release = chr_release,
    .unlocked_ioctl   = chr_ioctl,
};


static int __init chr_init(void)
{
    int ret;
    ret = register_chrdev(MAJOR_NUM, "chrdev", &chr_fops);
    if (ret < 0) {
        printk("regist failure!\n");
        return -1;
    }
    else {
        printk("test module loaded.\n");

        nfho_filter_input.hook = watch_filter;
        nfho_filter_input.owner = NULL;
        nfho_filter_input.pf = PF_INET;
        nfho_filter_input.priority = NF_IP_PRI_FIRST;

        nfho_filter_output.hook = watch_filter;
        nfho_filter_output.owner = NULL;
        nfho_filter_output.pf = PF_INET;
        nfho_filter_output.priority = NF_IP_PRI_FIRST;

        nfho_filter_forward.hook = watch_filter;
        nfho_filter_forward.owner = NULL;
        nfho_filter_forward.pf = PF_INET;
        nfho_filter_forward.priority = NF_IP_PRI_FIRST;

        nfho_filter_input.hooknum = NF_INET_LOCAL_IN;
        nfho_filter_output.hooknum = NF_INET_LOCAL_OUT;
        nfho_filter_forward.hooknum = NF_INET_FORWARD;

        nf_register_hook(&nfho_filter_input);
        nf_register_hook(&nfho_filter_output);
        nf_register_hook(&nfho_filter_forward);

        nfho_nat_prerouting.hook = watch_nat;
        nfho_nat_prerouting.owner = NULL;
        nfho_nat_prerouting.pf = PF_INET;
        nfho_nat_prerouting.priority = NF_IP_PRI_FIRST;

        nfho_nat_postrouting.hook = watch_nat;
        nfho_nat_postrouting.owner = NULL;
        nfho_nat_postrouting.pf = PF_INET;
        nfho_nat_postrouting.priority = NF_IP_PRI_FIRST;

        nfho_nat_prerouting.hooknum = NF_INET_PRE_ROUTING;
        nfho_nat_postrouting.hooknum =  NF_INET_POST_ROUTING;
        
        nf_register_hook(&nfho_nat_prerouting);
        nf_register_hook(&nfho_nat_postrouting);
    }
    return ret;
}

static void __exit chr_clean(void)
{
    unregister_chrdev(MAJOR_NUM, "chrdev");
    printk("test module exit.\n");
    
    nf_unregister_hook(&nfho_filter_input);
    nf_unregister_hook(&nfho_filter_output);
    nf_unregister_hook(&nfho_filter_forward);

    nf_unregister_hook(&nfho_nat_prerouting);
    nf_unregister_hook(&nfho_nat_postrouting);
}

module_init(chr_init);
module_exit(chr_clean);

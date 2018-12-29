#ifndef HOOK_H_
#define HOOK_H_

#include <net/ip.h>
#include <linux/ip.h>
#include <linux/udp.h>
#include <linux/time.h>
#include <linux/skbuff.h>
#include <linux/netfilter_ipv4.h>

#include "nat.h"
#include "utils.h"
#include "filter.h"

extern struct nf_hook_ops nfho_filter_input;
extern struct nf_hook_ops nfho_filter_output;
extern struct nf_hook_ops nfho_filter_forward;

extern struct nf_hook_ops nfho_nat_prerouting;
extern struct nf_hook_ops nfho_nat_postrouting;


unsigned int watch_filter(unsigned int hooknum,
			struct sk_buff *skb,
			const struct net_device *in,
			const struct net_device *out,
			int (*okfn)(struct sk_buff *));

unsigned int watch_nat(unsigned int hooknum,
			struct sk_buff *skb,
			const struct net_device *in,
			const struct net_device *out,
			int (*okfn)(struct sk_buff *));


#endif
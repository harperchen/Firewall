#include "hook.h"

#define MAX_COUNT 5

struct nf_hook_ops nfho_filter_input;
struct nf_hook_ops nfho_filter_output;
struct nf_hook_ops nfho_filter_forward;

struct nf_hook_ops nfho_nat_prerouting;
struct nf_hook_ops nfho_nat_postrouting;

static int packet_count = 0;


void check_ftp(struct linkItem *item, unsigned char *str, int len){
	__be16 port = 0;	
	char *temp, *prep, *data;
	int i, a = 0, b = 0;
	struct filterItem *filter;

	data = kmalloc(len + 1, GFP_ATOMIC);
	memset(data, 0, len + 1);
	memcpy(data, str, len);

	if(item->dport == 21){
		//FTP
		if(strstr(data, "PORT ")){
			data += strlen("PORT ");
			printk("%s\n", data);
			prep = data;
			temp = kmalloc(10, GFP_ATOMIC);
			memset(temp, 0, 10);
			for(i = 0; i < 5; i++){
				data = strchr(prep, ',');
				data++;
				if(i == 4){
					memcpy(temp, prep, strlen(prep) - strlen(data) - 1);
					sscanf(temp, "%d", &a);
				}
				prep = data;
			}
			memset(temp, 0, 10);
			memcpy(temp, data, strlen(data) - 2);
			sscanf(temp, "%d", &b);	
			port = a * 256 + b;
			kfree(temp);
			filter = kmalloc(sizeof(struct filterItem), GFP_ATOMIC);
			memset(filter, 0, sizeof(struct filterItem));
			//server 20 send syn to client
			filter->sip.from = filter->sip.to = item->daddr;
			filter->dip.from = filter->dip.to = item->saddr;
			filter->sport.from = filter->sport.to = 20;
			filter->dport.from = filter->dport.to = port;
			filter->protocol = IPPROTO_TCP;
			if(item->chain == NF_INET_LOCAL_IN){
				filter->chain = NF_INET_LOCAL_OUT;
			}
			else if(item->chain == NF_INET_LOCAL_OUT){
				filter->chain = NF_INET_LOCAL_IN;
			}
			else{
				filter->chain = NF_INET_FORWARD;
			}
			filter->action = true;
			filter->log = true;
			filter->index = item->index;

			firewall_printk(item->log, "FTP PORT MODE\n");
			firewall_printk(item->log, "Permit Source IP: %s Dest IP: %s ", hextoaddr(filter->sip.from), hextoaddr(filter->dip.from));
			firewall_printk(item->log, "Source Port: %d, Dest Port: %d ", filter->sport.from, filter->dport.from);

			line_table_put(FilterTable, filter);
		}
	}
	else if(item->sport == 21){				
		if(strstr(data, "227 Entering Passive Mode (")){
			data += strlen("227 Entering Passive Mode (");
			printk("%s\n", data);
			temp = kmalloc(10, GFP_ATOMIC);
			prep = data;
			for(i = 0; i < 5; i++){
				data = strchr(prep, ',');
				data++;
				if(i == 4){
					memset(temp, 0, 10);
					memcpy(temp, prep, strlen(prep) - strlen(data) - 1);
					sscanf(temp, "%d", &a);
				}
				prep = data;
			}
			data = strchr(prep, ')');
			memset(temp, 0, 10);
			memcpy(temp, prep, strlen(prep) - strlen(data));
			sscanf(temp, "%d", &b);		
			port = a * 256 + b;
			kfree(temp);
			filter = kmalloc(sizeof(struct filterItem), GFP_ATOMIC);
			memset(filter, 0, sizeof(struct filterItem));
			//client send syn to server
			filter->sip.from = filter->sip.to = item->daddr;
			filter->dip.from = filter->dip.to = item->saddr;
			filter->sport.from = 1024;
			filter->sport.to = 65535;
			filter->dport.from = filter->dport.to = port;
			filter->protocol = IPPROTO_TCP;
			filter->chain = item->chain;
			filter->action = true;
			filter->log = true;
			filter->index = item->index;

			firewall_printk(item->log, "FTP PASV MODE\n");
			firewall_printk(item->log, "Permit Source IP: %s Dest IP: %s ", hextoaddr(filter->sip.from), hextoaddr(filter->dip.from));
			firewall_printk(item->log, "Source Port: %d, Dest Port: %d ", filter->sport.from, filter->dport.from);

			line_table_put(FilterTable, filter);
		}
	}
}


unsigned int watch_filter(unsigned int hooknum,
			struct sk_buff *skb,
			const struct net_device *in,
			const struct net_device *out,
			int (*okfn)(struct sk_buff *)){
	char *str, *data;
	int search_ret = -1, data_len, tcp_len;
	bool isSyn, isAck, isFin;
	struct linkItem *item, *ret_item = NULL;
	struct filterItem* filter_item;
	
	struct iphdr *iph;
	struct tcphdr *tcph;
	struct udphdr *udph;
	struct icmphdr *icmph;
	struct timespec tsp;

	if(LinkTable == NULL){
		return defaultAction;
	}
	if(!skb){
		return defaultAction;
	}
	packet_count++;
	if(packet_count == MAX_COUNT){
		firewall_printk(false, "it's time to check link timeout\n");
		hash_table_update(LinkTable, &link_item_update);
		packet_count = 0;
	}
	printk("entering kernel start process\n");
	right_time();
	item = kmalloc(sizeof(struct linkItem), GFP_ATOMIC);
	memset(item, 0, sizeof(struct linkItem));

	iph = ip_hdr(skb);
	item->saddr = addr_change(iph->saddr);
	item->daddr = addr_change(iph->daddr);
	item->protocol = iph->protocol;

	if(iph->protocol == IPPROTO_TCP){
		tcph = (void *)iph + ip_hdrlen(skb);
		item->sport = ntohs(tcph->source);
		item->dport = ntohs(tcph->dest);
		item->seq = addr_change(tcph->seq);
		item->ack_seq = addr_change(tcph->ack_seq);
		isSyn = tcph->syn;
		isAck = tcph->ack;
		isFin = tcph->fin;

		if(isSyn && !isAck){
			str = link_to_str(item, true);
			ret_item = hash_table_get(LinkTable, str);
			if(!ret_item){				
				search_ret = line_table_search(FilterTable, item, hooknum);
				if(search_ret == -1){
					if(defaultAction == NF_DROP){
						kfree(str);
						printk("out kernel end process\n");
						right_time();
						return defaultAction;
					}
					item->log = false;
					item->index = -1;
					item->chain = hooknum;
				}
				else{
					filter_item = FilterTable->table[search_ret];
					if(!(filter_item->action)){
						kfree(str);
						firewall_printk(filter_item->log, "\n");
						firewall_printk(filter_item->log, "search TCP SYN packet in filter table but policy is drop\n");
						firewall_printk(filter_item->log, "TCP...saddr %s daddr %s sport: %d dport: %d seq: %u ack_seq: %u\n\n", 
							hextoaddr(item->saddr), hextoaddr(item->daddr), item->sport, item->dport, item->seq, item->ack_seq);
						printk("out kernel end process\n");
						right_time();
						return NF_DROP;
					}
					firewall_printk(filter_item->log, "\n");
					firewall_printk(filter_item->log, "search TCP SYN packet in filter table return accept\n");
					item->log = filter_item->log;
					item->index = filter_item->index;
					item->chain = filter_item->chain;
				}
			}
			else{
				item->log = ret_item->log;
            	item->index = ret_item->index;
				item->chain = ret_item->chain;
			}
			item->tcp_status = SYN_SENT;
			firewall_printk(item->log, "put into link table\n");
		}
		else if(isSyn && isAck){
			//SYN&ACK
			str = link_to_str(item, false);
			ret_item = hash_table_get(LinkTable, str);
			if(!ret_item){
				kfree(str);
				printk("out kernel end process\n");
				right_time();
				return defaultAction;
			}
            item->tcp_status = ESTABLISHED;
            firewall_printk(ret_item->log, "\n");
            firewall_printk(ret_item->log, "find TCP SYN&ACK packet in link table\n");
		}
		else{
			//别的带有ACK标志的报文
			str = link_to_str(item, false);
			ret_item = hash_table_get(LinkTable, str);
			if(!ret_item){
				kfree(str);
				str = link_to_str(item,true);
				ret_item = hash_table_get(LinkTable, str);
				if(!ret_item){
					kfree(str);
					printk("out kernel end process\n");
					right_time();
					return defaultAction;
				}
			}
			firewall_printk(ret_item->log, "\n");
			if(ret_item->tcp_status == FIN_WAIT_1){
				firewall_printk(ret_item->log, "find TCP FIN_WAIT_1 packet in link table\n");
				item->tcp_status = FIN_WAIT_2;
				if(isFin){
					firewall_printk(ret_item->log, "TCP...saddr %s daddr %s sport: %d dport: %d seq: %u ack_seq: %u\n\n", 
						hextoaddr(item->saddr), hextoaddr(item->daddr), item->sport, item->dport, item->seq, item->ack_seq);
					firewall_printk(ret_item->log, "find TCP FIN_WAIT_2 packet in link table\n");
					firewall_printk(ret_item->log, "receive fin packet, change status into TIME_WAIT\n");
					item->tcp_status = TIME_WAIT_1;
				}
			}
			else if(ret_item->tcp_status == TIME_WAIT_1){
				//删掉此条
				firewall_printk(ret_item->log, "find TCP TIME_WAIT packet in link table\n");
				firewall_printk(ret_item->log, "remove link item from link table\n");
				hash_table_rm(LinkTable, str);
				printk("out kernel end process\n");
				right_time();
				return NF_ACCEPT;
			}
			else if((ret_item->tcp_status == FIN_WAIT_2) && isFin){
				firewall_printk(ret_item->log, "find TCP FIN_WAIT_2 packet in link table\n");
				firewall_printk(ret_item->log, "receive fin packet, change status into TIME_WAIT\n");
				item->tcp_status = TIME_WAIT_1;
			}
			else if((ret_item->tcp_status == ESTABLISHED) && isFin){
				firewall_printk(ret_item->log, "find TCP ESTABLISHED packet in link table\n");
				firewall_printk(ret_item->log, "receive fin packet, change status into FIN_WAIT_1\n");
				item->tcp_status = FIN_WAIT_1;
			}
			else if(ret_item->tcp_status == SYN_SENT) {
				kfree(str);
				printk("out kernel end process\n");
				right_time();
				return defaultAction;
			}
			else {
				firewall_printk(ret_item->log, "find TCP ESTABLISHED packet in link table\n");
				item->tcp_status = ESTABLISHED;
				if((item->dport == 21) || (item->sport == 21)){
					//FTP
			        tcp_len = tcph->doff * 4;
			        data = (unsigned char *)tcph + tcp_len;
			        data_len = skb->len - iph->ihl * 4 - tcp_len;
			        item->chain = ret_item->chain;
					check_ftp(item, data, data_len);
				}
			}
		}
        if(ret_item){
            item->log = ret_item->log;
            item->index = ret_item->index;
			item->chain = ret_item->chain;
        }
        firewall_printk(item->log, "TCP...saddr %s daddr %s sport: %d dport: %d seq: %u ack_seq: %u\n\n", 
						hextoaddr(item->saddr), hextoaddr(item->daddr), item->sport, item->dport, item->seq, item->ack_seq);
		tsp = current_kernel_time();
		item->timeout = tsp.tv_sec;
		hash_table_put(LinkTable, str, item);
		printk("out kernel end process\n");
		right_time();
		return NF_ACCEPT;
	}
	else if(iph->protocol == IPPROTO_UDP){
		udph = (void *)iph + ip_hdrlen(skb);
		item->sport = ntohs(udph->source);
		item->dport = ntohs(udph->dest);
		str = link_to_str(item, false);
		ret_item = hash_table_get(LinkTable, str);
		if(!ret_item){
			kfree(str);
			str = link_to_str(item,true);
			ret_item = hash_table_get(LinkTable, str);
			if(!ret_item){
				search_ret = line_table_search(FilterTable, item, hooknum);
				if(search_ret == -1){
					if(defaultAction == NF_DROP){
						kfree(str);
						printk("out kernel end process\n");
						right_time();
						return defaultAction;
					}
					item->log = false;
					item->index = -1;
					item->chain = hooknum;
				}
				else{
					filter_item = FilterTable->table[search_ret];
					firewall_printk(filter_item->log, "\n");
					if(!(filter_item->action)){
						firewall_printk(filter_item->log, "search UDP packet in filter table but policy is drop\n");
						firewall_printk(filter_item->log, "UDP...saddr %s daddr %s sport: %d dport: %d\n\n", hextoaddr(item->saddr), hextoaddr(item->daddr), item->sport, item->dport);
						kfree(str);
						return NF_DROP;
					}
					firewall_printk(filter_item->log, "search UDP packet in filter table return accept\n");
					firewall_printk(filter_item->log, "put into link table\n");
					item->log = filter_item->log;
					item->index = filter_item->index;
					item->chain = filter_item->chain;
				}
			}
		}
		if(ret_item){
			firewall_printk(ret_item->log, "\n");
			firewall_printk(ret_item->log, "find UDP packet in link table\n");
			item->log = ret_item->log;
			item->index = ret_item->index;
			item->chain = ret_item->chain;
		}
		firewall_printk(item->log, "UDP...saddr %s daddr %s sport: %d dport: %d\n\n", hextoaddr(item->saddr), hextoaddr(item->daddr), item->sport, item->dport);
		tsp = current_kernel_time();
		item->timeout = tsp.tv_sec;
		hash_table_put(LinkTable, str, item);
		return NF_ACCEPT;
	}
	else if(iph->protocol == IPPROTO_ICMP){
		icmph = (void *)iph + ip_hdrlen(skb);
		item->type = icmph->type;
		if((item->type != ICMP_ECHO) && (item->type != ICMP_ECHOREPLY)){
			kfree(item);
			printk("out kernel end process\n");
			right_time();
			return defaultAction;
		}
		if(item->type == ICMP_ECHO){
			str = link_to_str(item, true);
			ret_item = hash_table_get(LinkTable, str);
			if(!ret_item){
				search_ret = line_table_search(FilterTable, item, hooknum);
				if(search_ret == -1){
					if(defaultAction == NF_DROP){
						kfree(str);
						printk("out kernel end process\n");
						right_time();
						return defaultAction;
					}
					item->log = false;
					item->index = -1;
					item->chain = hooknum;
				}
				else{
					filter_item = FilterTable->table[search_ret];
					if(!(filter_item->action)){
						kfree(str);
						firewall_printk(filter_item->log, "\n");
						firewall_printk(filter_item->log, "search ICMP_ECHO packet in filter table but policy is drop\n");
						firewall_printk(filter_item->log, "ICMP...saddr %s daddr %s icmp type %d\n\n", hextoaddr(item->saddr), hextoaddr(item->daddr), item->type);
						printk("out kernel end process\n");
						right_time();
						return NF_DROP;
					}
					item->log = filter_item->log;
					item->index = filter_item->index;
					item->chain = filter_item->chain;
					firewall_printk(item->log, "\n");
					firewall_printk(item->log, "search ICMP_ECHO packet in filter table ret accept\n");
					firewall_printk(item->log, "put into link table\n");
				}
			}
		}
		else if(item->type == ICMP_ECHOREPLY){
			str = link_to_str(item, false);
			ret_item = hash_table_get(LinkTable, str);
			if(!ret_item){
				kfree(str);
				kfree(item);
				printk("out kernel end process\n");
				right_time();
				return defaultAction;
			}
		}
		else{
			kfree(item);
			printk("out kernel end process\n");
			right_time();
			return defaultAction;
		}
		if(ret_item){
			item->log = ret_item->log;
			item->index = ret_item->index;
			item->chain = ret_item->chain;
			firewall_printk(item->log, "\n");
			if(item->type == ICMP_ECHO){
				firewall_printk(item->log, "find ICMP_ECHO packet in link table\n");
			}
			else{
				firewall_printk(item->log, "find ICMP_ECHO REPLY packet in link table\n");
			}
		}
		firewall_printk(item->log, "ICMP...saddr %s daddr %s icmp type %d\n\n", hextoaddr(item->saddr), hextoaddr(item->daddr), item->type);
		tsp = current_kernel_time();
		item->timeout = tsp.tv_sec;
		hash_table_put(LinkTable, str, item);
		printk("out kernel end process\n");
		right_time();
		return NF_ACCEPT;
	}
	kfree(item);
	printk("out kernel end process\n");
	right_time();
	return defaultAction;
}

unsigned int watch_nat(unsigned int hooknum,
			struct sk_buff *skb,
			const struct net_device *in,
			const struct net_device *out,
			int (*okfn)(struct sk_buff *)){
	int search_ret;
	__be32 saddr, daddr, addr;
	__be16 sport, dport, port;
	struct natSearch *nat, *dyn;
	struct natItem *rule;
	struct mapItem *map;

	struct iphdr *iph = NULL;
	struct tcphdr *tcph = NULL;
	struct udphdr *udph = NULL;
	struct icmphdr *icmph = NULL;
 
	sport = dport = 0;

	if(MapTable == NULL){
		return NF_ACCEPT;
	}
	if(!skb){
		return NF_ACCEPT;
	}
	nat = kmalloc(sizeof(struct natSearch), GFP_ATOMIC);
	memset(nat, 0, sizeof(struct natSearch));

	printk("\n");
	iph = ip_hdr(skb);
	saddr = addr_change(iph->saddr);
	daddr = addr_change(iph->daddr);
	nat->protocol = iph->protocol;
	
	if(iph->protocol == IPPROTO_TCP){
		tcph = (void *)iph + ip_hdrlen(skb);
		sport = ntohs(tcph->source);
		dport = ntohs(tcph->dest);
	}
	else if(iph->protocol == IPPROTO_UDP){
		udph = (void *)iph + ip_hdrlen(skb);
		sport = ntohs(udph->source);
		dport = ntohs(udph->dest);
	}
	else if(iph->protocol == IPPROTO_ICMP){
		icmph = (void *)iph + ip_hdrlen(skb);
	}
	else{
		kfree(nat);
		return NF_ACCEPT;
	}

	if(hooknum == NF_INET_PRE_ROUTING){
		nat->addr = daddr;
		nat->port = dport;
		//search daddr
	}
	else{
		nat->addr = saddr;
		nat->port = sport;
		//search saddr
	}
	
	search_ret = line_table_search(MapTable, nat, 0);

	if(search_ret == -1){
		//Map表里没找到
		search_ret = line_table_search(NatTable, nat, 0);
		if(search_ret == -1){
			kfree(nat);
			return NF_ACCEPT;
		}
		rule = NatTable->table[search_ret];
		map = kmalloc(sizeof(struct mapItem), GFP_ATOMIC);
		memset(map, 0, sizeof(struct mapItem));
		map->before_ip = nat->addr;
		map->type = rule->type;
		map->index = rule->index;

		firewall_printk(true, "\n");
		if(rule->type == STATIC){	
			//静态NAT IP地址一对一
			map->after_ip = rule->after_ip.from;
			if(hooknum == NF_INET_PRE_ROUTING){
				iph->daddr = addr_change(map->after_ip);
				firewall_printk(true, "Destination static Nat: before ip is %s, after_ip is %s\n", hextoaddr(map->before_ip), hextoaddr(map->after_ip));
				//search daddr
			}
			else{
				iph->saddr = addr_change(map->after_ip);
				firewall_printk(true, "Source static Nat: before ip is %s, after_ip is %s\n", hextoaddr(map->before_ip), hextoaddr(map->after_ip));
				//search saddr
			}
		}
		else if(rule->type == DYNAMIC){
			//动态NAT IP一对多
			dyn = kmalloc(sizeof(struct natSearch), GFP_ATOMIC);
			memset(dyn, 0, sizeof(struct natSearch));
			dyn->protocol = IPPROTO_ICMP;
			for(addr = rule->after_ip.to - 1; addr > rule->after_ip.from; addr--){	
				dyn->addr = addr;
				search_ret = line_table_search(MapTable, dyn, 0);
				if(search_ret == -1){
					break;
				}
			}
			if(search_ret != -1){
				//ip 分配完了
				kfree(dyn);
				return NF_ACCEPT;
			}
			kfree(dyn);
			map->after_ip = addr;
			if(hooknum == NF_INET_PRE_ROUTING){
				iph->daddr = addr_change(addr);
				firewall_printk(true, "Destination dynamic Nat: before ip is %s, after_ip is %s\n", hextoaddr(map->before_ip), hextoaddr(map->after_ip));
				//search daddr
			}
			else{
				iph->saddr = addr_change(addr);
				firewall_printk(true, "Source dynamic Nat: before ip is %s, after_ip is %s\n", hextoaddr(map->before_ip), hextoaddr(map->after_ip));
				//search saddr
			}
		}
		else{
			//端口NAT
			printk("test\n");
			map->before_port = nat->port;
			map->after_ip = rule->after_ip.from;
			dyn = kmalloc(sizeof(struct natSearch), GFP_ATOMIC);
			memset(dyn, 0, sizeof(struct natSearch));
			dyn->protocol = IPPROTO_TCP;
			dyn->addr = rule->after_ip.from;
			if(rule->after_port.from < 1024){
				rule->after_port.from = 1024;
			}
			for(port = rule->after_port.from; port <= rule->after_port.to; port++){
				dyn->port = port;
				search_ret = line_table_search(MapTable, dyn, 0);
				if(search_ret == -1){
					break;
				}
			}
			if(search_ret != -1){
				//port 分配完了
				kfree(dyn);
				return NF_ACCEPT;
			}
			kfree(dyn);
			map->after_port = port;
			if(hooknum == NF_INET_PRE_ROUTING){
				//search daddr
				iph->daddr = addr_change(map->after_ip);
				if(iph->protocol == IPPROTO_TCP){
					tcph->dest = htons(port);
				}
				else if(iph->protocol == IPPROTO_UDP){
					udph->dest = htons(port);
				}
				firewall_printk(true, "Destination pat Nat: before ip is %s, after_ip is %s\n", hextoaddr(map->before_ip), hextoaddr(map->after_ip));
				if(iph->protocol != IPPROTO_ICMP){
					firewall_printk(true, "Destination pat Nat: before port is %d, after_port is %d\n", map->before_port, map->after_port);
				}
			}
			else{
				iph->saddr = addr_change(map->after_ip);
				//search saddr
				if(iph->protocol == IPPROTO_TCP){
					tcph->source = htons(port);
				}
				else if(iph->protocol == IPPROTO_UDP){
					udph->source = htons(port);
				}
				firewall_printk(true, "Source pat Nat: before ip is %s, after_ip is %s\n", hextoaddr(map->before_ip), hextoaddr(map->after_ip));
				if(iph->protocol != IPPROTO_ICMP){
					firewall_printk(true, "Source pat Nat: before port is %d, after_port is %d\n", map->before_port, map->after_port);
				}
			}
		}
		line_table_put(MapTable, map);
	}
	else{
		//超时更新
		map = MapTable->table[search_ret];
		if(map->before_ip == nat->addr){
			addr = map->after_ip;
			port = map->after_port;
		}
		else{
			addr = map->before_ip;
			port = map->before_port;
		}
		firewall_printk(true, "\n");
		if(hooknum == NF_INET_PRE_ROUTING){
			if(map->type == STATIC){
				firewall_printk(true, "Destination static Nat: before ip is %s, after_ip is %s\n", hextoaddr(map->before_ip), hextoaddr(map->after_ip));
			}
			else if(map->type == DYNAMIC){
				firewall_printk(true, "Destination dynamic Nat: before ip is %s, after_ip is %s\n", hextoaddr(map->before_ip), hextoaddr(map->after_ip));
			}
			else{
				firewall_printk(true, "Destination pat Nat: before ip is %s, after_ip is %s\n", hextoaddr(map->before_ip), hextoaddr(map->after_ip));
				if(iph->protocol != IPPROTO_ICMP){
					firewall_printk(true, "Destination pat Nat: before port is %d, after_port is %d\n", map->before_port, map->after_port);
				}
			}
			//search daddr
			iph->daddr = addr_change(addr);
			if((iph->protocol == IPPROTO_TCP) && (map->type == PAT)){
				tcph->dest = htons(port);
			}
			else if((iph->protocol == IPPROTO_UDP) && (map->type == PAT)){
				udph->dest = htons(port);
			}
		}
		else{
			if(map->type == STATIC){
				firewall_printk(true, "Source static Nat: before ip is %s, after_ip is %s\n", hextoaddr(map->before_ip), hextoaddr(map->after_ip));
			}
			else if(map->type == DYNAMIC){
				firewall_printk(true, "Source dynamic Nat: before ip is %s, after_ip is %s\n", hextoaddr(map->before_ip), hextoaddr(map->after_ip));
			}
			else{
				firewall_printk(true, "Source pat Nat: before ip is %s, after_ip is %s\n", hextoaddr(map->before_ip), hextoaddr(map->after_ip));
				if(iph->protocol != IPPROTO_ICMP){
					firewall_printk(true, "Source pat Nat: before port is %d, after_port is %d\n", map->before_port, map->after_port);
				}
			}
			iph->saddr = addr_change(addr);
			//search saddr
			if((iph->protocol == IPPROTO_TCP) && (map->type == PAT)){
				tcph->source = htons(port);
			}
			else if((iph->protocol == IPPROTO_UDP) && (map->type == PAT)){
				udph->source = htons(port);
			}
		}

	}
	//计算校验和 UDP TCP ICMP
	iph->check = 0;
    iph->check = ip_fast_csum((unsigned char*)iph, iph->ihl);//计算校验和
	if(iph->protocol == IPPROTO_TCP){
		tcph->check = 0;
		skb->csum = csum_partial(tcph, ntohs(iph->tot_len) - iph->ihl * 4, 0);
		tcph->check = csum_tcpudp_magic(iph->saddr, iph->daddr, (ntohs(iph->tot_len)- iph->ihl * 4), IPPROTO_TCP, skb->csum); 
		
	}
	else if(iph->protocol == IPPROTO_UDP){
		udph->check = 0;
		//skb->csum = csum_partial(udph, ntohs(iph->tot_len) - iph->ihl * 4, 0);
		skb->csum = skb_checksum(skb, iph->ihl * 4, skb->len - iph->ihl * 4, 0);//skb校验和计算
		udph->check = csum_tcpudp_magic(iph->saddr, iph->daddr, (ntohs(iph->tot_len)- iph->ihl * 4), IPPROTO_UDP, skb->csum);
    }
	else if(iph->protocol == IPPROTO_ICMP){
		icmph->checksum = 0;
		icmph->checksum = ip_compute_csum((unsigned short *)icmph,  
		                            ntohs(iph->tot_len) - (iph->ihl << 2));
	}
	skb_reset_transport_header(skb);
	kfree(nat);
	return NF_ACCEPT;
}

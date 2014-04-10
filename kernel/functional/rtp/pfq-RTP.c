#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>

#include <linux/pf_q.h>
#include <linux/pf_q-module.h>


MODULE_LICENSE("GPL");


/* Basic RTP header */
struct rtphdr {
	uint8_t  rh_flags;	/* T:2 P:1 X:1 CC:4 */
	uint8_t  rh_pt;		/* M:1 PT:7 */
	uint16_t rh_seqno;	/* sequence number */
	uint32_t rh_ts;		/* media-specific time stamp */
	uint32_t rh_ssrc;	/* synchronization src id */
	/* data sources follow per cc */
};

struct rtcphdr {
	uint8_t  rh_flags;	/* T:2 P:1 CNT:5 */
	uint8_t  rh_type;	/* type */
	uint16_t rh_len;	/* length of message (in bytes) */
	uint32_t rh_ssrc;	/* synchronization src id */
};

/* Rtp/Rtcp packet */

struct headers {
	struct udphdr udp;
	union
	{
		struct rtphdr 	rtp;
		struct rtcphdr  rtcp;
	} un;

} __attribute__((packed));


static inline
bool valid_codec(uint8_t c)
{
	return c < 19  ? true :
	       c > 95  ? true :
	       c == 25 ? true :
	       c == 26 ? true :
	       c == 28 ? true :
	       c == 31 ? true :
	       c == 32 ? true :
	       c == 33 ? true :
	       c == 34 ? true : false;
}


struct sk_buff *
heuristic_rtp(struct sk_buff *skb, argument_t a, bool steer)
{
	if (eth_hdr(skb)->h_proto == __constant_htons(ETH_P_IP))
	{
		struct iphdr _iph;
    		const struct iphdr *ip;

		struct headers _hdr;
		const struct headers *hdr;

                uint16_t source,dest;

		ip = skb_header_pointer(skb, skb->mac_len, sizeof(_iph), &_iph);
 		if (ip == NULL)
        		return drop(skb);

		if (ip->protocol != IPPROTO_UDP)
        		return drop(skb);

		hdr = skb_header_pointer(skb, skb->mac_len + (ip->ihl<<2), sizeof(_hdr), &_hdr);
		if (hdr == NULL)
        		return drop(skb);

		/* version => 2 */

		if (!((ntohs(hdr->un.rtp.rh_flags) & 0xc000) == 0x8000))
        		return drop(skb);

		dest   = ntohs(hdr->udp.dest);
		source = ntohs(hdr->udp.source);

		if (dest < 1024 || source < 1024)
        		return drop(skb);

		if ((dest & 1) && (source & 1))  /* rtcp */
		{
                	if (hdr->un.rtcp.rh_type != 200)  /* SR  */
        			return drop(skb);
		}
		else if (!((dest & 1) || (source & 1)))
		{
                	uint8_t pt = hdr->un.rtp.rh_pt;
                 	if (!valid_codec(pt))
        			return drop(skb);
		}

		return steer ? steering(skb, ip->saddr ^ ip->daddr ^ ((uint32_t)(hdr->udp.source & 0xfffe) << 16) ^ (hdr->udp.dest & 0xfffe)) : skb;

	}

        return drop(skb);
}


struct sk_buff *
filter_rtp(struct sk_buff *skb, argument_t a)
{
        return heuristic_rtp(skb, a, false);
}


struct sk_buff *
steering_rtp(struct sk_buff *skb, argument_t a)
{
        return heuristic_rtp(skb, a, true);
}


struct pfq_function_descr hooks[] = {

	{ "rtp",       filter_rtp   },
	{ "steer-rtp", steering_rtp },
	{ NULL, NULL}};


static int __init usr_init_module(void)
{
	return pfq_symtable_register_functions("[RTP]", &pfq_monadic_cat, hooks);
}


static void __exit usr_exit_module(void)
{
	pfq_symtable_unregister_functions("[RTP]", &pfq_monadic_cat, hooks);
}


module_init(usr_init_module);
module_exit(usr_exit_module);



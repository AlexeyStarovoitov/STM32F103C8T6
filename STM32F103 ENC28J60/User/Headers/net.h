#ifndef NET_H_
#define NET_H_

#define be16toword(a) ((((a)>>8)&0xff)|(((a)<<8)&0xff00))

#define ARP_TYPE ((uint16_t)be16toword(0x0806))
#define IP_TYPE ((uint16_t)be16toword(0x0800))

#define ARP_IP ((uint16_t)be16toword(0x0800))
#define ARP_ETH ((uint16_t)be16toword(0x0001))
#define ARP_REQ ((uint16_t)be16toword(0x0001))
#define ARP_REP ((uint16_t)be16toword(0x0002))

#define IP_ICMP	((uint8_t)1)
#define IP_TCP	((uint8_t)6)
#define IP_UDP	((uint8_t)17) 
#define IP_VERLEN	((uint8_t)0x45) 

#define ICMP_REQ ((uint8_t)0x08)
#define ICMP_REP ((uint8_t)0x00)

#define STR_LEN ((uint16_t)80)

typedef struct
{
	uint16_t net_type;
	uint16_t prot_type;
	uint8_t mac_len;
	uint8_t ip_len;
	uint16_t req_type;
	uint8_t mac_src[6];
	uint8_t ip_src[4];
	uint8_t mac_dest[6];
	uint8_t ip_dest[4];
} arp_pckt;

typedef struct
{
	uint8_t ver_hlen;
	uint8_t tos;
	uint16_t pack_len;
	uint16_t id;
	uint16_t flgs_fr_off;
	uint8_t ttl;
	uint8_t prot_id;
	uint16_t ch_sum;
	uint8_t ip_src[4];
	uint8_t ip_dest[4];
	
}ip_header;

typedef struct
{
	ip_header *hdr;
	uint16_t dat_len;
	uint8_t *data;
}
ip_pckt;

typedef struct
{
	uint8_t msg_tp;
	uint8_t msg_cd;
	uint16_t ch_sum;
	uint16_t pck_id;
	uint16_t pck_num;
} icmp_header;

typedef struct
{
	icmp_header *hdr;
	uint16_t dat_len;
	uint8_t *data;
} icmp_pckt;



void net_init(void);
uint16_t ip_ch_sum_calc(uint8_t *head, uint16_t len);
void eth_send(enc28_frame *frame_rec, uint16_t dat_len);
void arp_send(enc28_frame *frame_rec,  uint16_t arp_pckt_len);
uint8_t arp_read(enc28_frame *frame_rec, uint16_t len);
uint8_t ip_send(enc28_frame *frame_rec, uint16_t ip_dat_len);
uint8_t ip_read(enc28_frame *frame_rec, uint16_t ip_len);
uint8_t icmp_read(enc28_frame *frame_rec, uint16_t icmp_pckt_len);
void net_poll(void);


#endif

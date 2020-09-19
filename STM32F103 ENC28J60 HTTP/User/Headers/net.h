#ifndef NET_H_
#define NET_H_

#define be16toword(a) ((((a)>>8)&0xff)|(((a)<<8)&0xff00))
#define be32todword(a) ((((a)>>24)&0xff)| (((a)>>8)&0xff00)| (((a)<<8)&0xff0000) | (((a)<<24)&0xff000000))

#define ARP_TYPE ((uint16_t)be16toword(0x0806))
#define IP_TYPE ((uint16_t)be16toword(0x0800))

#define ARP_IP ((uint16_t)be16toword(0x0800))
#define ARP_ETH ((uint16_t)be16toword(0x0001))
#define ARP_REQ ((uint16_t)be16toword(0x0001))
#define ARP_REP ((uint16_t)be16toword(0x0002))

#define ARP_TABLE_LEN ((uint8_t)5)
#define CLK_TIM_OUT ((uint32_t)43200)

#define IP_ICMP	((uint8_t)1)
#define IP_TCP	((uint8_t)6)
#define IP_UDP	((uint8_t)17) 
#define IP_VERLEN	((uint8_t)0x45) 

#define ICMP_REQ ((uint8_t)0x08)
#define ICMP_REP ((uint8_t)0x00)

#define STR_LEN ((uint16_t)80)

#define UART_UDP_RQST ((uint8_t)0x01)

#define UDP_PORT_SRC ((uint16_t)333)
#define TCP_PORT_SRC ((uint16_t)80)

// TCP Flags

#define TCP_FIN ((uint8_t)0x01)
#define TCP_SYN ((uint8_t)0x02)
#define TCP_RST ((uint8_t)0x04)
#define TCP_PSH ((uint8_t)0x08)
#define TCP_ACK ((uint8_t)0x10)
#define TCP_URG ((uint8_t)0x20)
#define TCP_ECE ((uint8_t)0x40)
#define TCP_CWR ((uint8_t)0x80)

//TCP Operations

#define TCP_OP_SYNACK 1
#define TCP_OP_ACK_OF_FIN 2
#define TCP_OP_ACK_OF_RST 3
#define TCP_OP_ACK_OF_DATA 4

// TCP Status

#define TCP_CONNECTED ((uint8_t)1)
#define TCP_DISCONNECTED ((uint8_t)2)
#define TCP_DISCONNECTING ((uint8_t)3)

// TCP Data status

#define TCP_DATA_COMPLETED ((uint8_t)0)
#define TCP_DATA_ONE ((uint8_t)1)
#define TCP_DATA_FIRST ((uint8_t)2)
#define TCP_DATA_MIDDLE ((uint8_t)3)
#define TCP_DATA_LAST ((uint8_t)4)
#define TCP_DATA_END ((uint8_t)5)


#define TCP_WIN_LEN ((uint16_t)8192)
#define TCP_SEG_LEN ((uint16_t)458)
//#define TCP_SEG_LEN ((uint16_t)1460)

// Check Sum
#define UDP_CH_SUM ((uint8_t)1)
#define TCP_CH_SUM ((uint8_t)2)

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
	uint16_t data_len;
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
	uint16_t data_len;
	uint8_t *data;
} icmp_pckt;

typedef struct
{
	uint16_t src_prt;
	uint16_t dst_prt;
	uint16_t pck_len;
	uint16_t ch_sum;
	
} udp_header;


typedef struct
{	
	udp_header *hdr;
	uint16_t data_len;
	uint8_t *data;
} udp_pckt;

typedef struct
{
	uint16_t src_prt;
	uint16_t dst_prt;
	uint32_t sn;
	uint32_t ack_sn;
	uint8_t hdr_len;
	uint8_t flgs;
	uint16_t win_size;
	uint16_t ch_sum;
	uint16_t urg_ptr;
} tcp_header;

typedef struct
{	
	tcp_header *hdr;
	uint16_t data_len;
	uint8_t *data;
} tcp_pckt;

typedef struct
{
	uint16_t dst_port;
	uint32_t sn;
	uint32_t ack_sn;
	uint8_t tcp_stat;
	uint8_t data_stat;
	uint32_t data_size;
	uint16_t last_data_size;
	uint16_t data_cnt;
}tcp_prop;

typedef struct
{
	uint8_t ip_buf[STR_LEN];
	uint8_t buf_len;
	uint8_t ip_rec;
	
} uart_ip;

typedef struct
{
	uint8_t mac_addr[6];
	uint8_t ip_addr[4];
	uint32_t clk;
} arp_record;



void net_init(void);
void ip_str2num(char *ip_str, uint8_t *ip_num);
uint16_t port_str2num(char *ip_str);
uint16_t ip_ch_sum_calc(uint8_t *head, uint16_t len, uint8_t type);
void eth_reply(enc28_frame *frame_repl, uint16_t dat_len);
void eth_request(enc28_frame *frame_req, uint16_t prot_type, uint8_t *mac_dest, uint16_t dat_len);
void arp_reply(enc28_frame *frame_rec,  uint16_t arp_pckt_len);
uint8_t arp_request (uint8_t *ip_dst);
void fill_arp_table(arp_record *arp_tbl_ptr, uint8_t *cur_len, uint8_t arp_tbl_len, uint32_t clk, enc28_frame *frame_rec);
int8_t find_arp_table_data(arp_record *arp_tbl_ptr, uint8_t cur_len, uint8_t *ip_num);
void show_arp_table(arp_record *arp_tbl_ptr, uint8_t cur_len);
void dlt_arp_table_record(arp_record *arp_tbl_ptr, uint8_t dlt_index, uint8_t *cur_len);
uint8_t arp_read(enc28_frame *frame_rec, uint16_t len);
uint8_t ip_reply(enc28_frame *frame_repl, uint16_t ip_dat_len);
void ip_request(enc28_frame *frame_req, uint8_t prot, uint8_t *mac_dest, uint8_t *ip_dest, uint16_t dat_len);
uint8_t ip_read(enc28_frame *frame_rec, uint16_t ip_len);
uint8_t udp_read(enc28_frame *frame_rec, uint16_t udp_pckt_len);
void udp_reply(enc28_frame *frame_rec);
void udp_request(uint8_t *mac_dest, uint8_t *ip_dest, uint16_t port_dest, uint8_t *data, uint16_t dat_len);
uint8_t tcp_read(enc28_frame *frame_rec, uint16_t tcp_pckt_len);
void tcp_header_prepare(tcp_pckt *tcp_repl, tcp_prop *tcp_prop, uint8_t flgs, uint16_t tcp_header_len, uint16_t tcp_data_len);
void tcp_reply(enc28_frame *frame_rec, uint8_t op_type, uint16_t tcp_pckt_len);
void tcp_http_data_send_one(enc28_frame *frame_rec, tcp_prop *tcp_prop, uint16_t tcp_pckt_len);
void tcp_http_data_end(enc28_frame *frame_rec, tcp_prop *tcp_prop);
uint8_t icmp_read(enc28_frame *frame_rec, uint16_t icmp_pckt_len);
void icmp_reply(enc28_frame *frame_rec, uint16_t icmp_pckt_len);
void net_poll(void);


#endif

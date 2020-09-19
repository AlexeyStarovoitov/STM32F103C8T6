#include "stm32f10x.h"
#include "gpio.h"
#include "enc28j60.h"
#include "spi.h"
#include "uart.h"
#include "net.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

extern uint8_t mac_addr[6];
extern uint8_t mac_addr_null[6];
extern uint8_t mac_addr_bc[6];
extern uint8_t ip_addr[4];
extern uint8_t ip_dst[4];
extern enc28_frame frame;
extern uint32_t clk_s; 

uint16_t dst_prt;
arp_record arp_table[ARP_TABLE_LEN];
uint8_t cur_index;
uart_ip ip_dst_str;

void net_init(void)
{
	frame.data = NULL;
	frame.data_len = 0;
	
	ip_dst_str.buf_len = 0;
	ip_dst_str.ip_rec = 0;
	
	cur_index = 0;
	clk_s = 0;
	uart_transmit((uint8_t *)"Start\r\n", 7);
}

void ip_str2num(char *ip_str, uint8_t *ip_num)
{
	uint8_t i, prt_len;
	char prt_str[5];
	char *dot_ptr, *sc_ptr;
	
	for(i = 0; i < 3; i++)
	{
		dot_ptr = strchr(ip_str, '.');
		
		prt_len = dot_ptr - ip_str+1;
		strncpy(prt_str, ip_str, prt_len);
		prt_str[prt_len] = 0;
		ip_num[i] = atoi(prt_str);
		ip_str = dot_ptr+1;
		
	}
	
	sc_ptr = strchr(ip_str, ':');
	if(sc_ptr)
	{
		prt_len = sc_ptr - ip_str+1;
		strncpy(prt_str, ip_str, prt_len);
		prt_str[prt_len] = 0;
		ip_num[i] = atoi(prt_str);
	}
	else
		ip_num[i] = atoi(ip_str);
	

}


uint16_t port_str2num(char *ip_str)
{
	uint16_t port;
	char *dot_ptr;
	
	dot_ptr = strchr(ip_str, ':');
	port = atoi(dot_ptr + 1);

	return port;
}

uint16_t ip_ch_sum_calc(uint8_t *head, uint16_t len, uint8_t type)
{
	
	uint32_t sum = 0;
	
	if(type == UDP_CH_SUM)
	{
		sum += IP_UDP;
		sum += len - 8;
	}
	else if(type == TCP_CH_SUM)
	{
		sum += IP_TCP;
		sum += len - 8;
	}
	
	while(len > 1)
	{
		sum += (uint16_t)(((uint16_t)*head << 8) | ((uint16_t)*(head+1)));
		len -=2;
		head += 2;
	}
	
	if(len)
		sum += (uint16_t)(((uint16_t)*head << 8) | ((uint16_t)0x00));
	
	while(sum & 0xFFFF0000)
		sum = ((sum >> 16) & 0x0000FFFF) + (sum & 0x0000FFFF);
	
	return ~be16toword(sum);
}

void eth_reply(enc28_frame *frame_repl, uint16_t dat_len)
{
	memcpy(frame_repl->hdr.dest_addr,   frame_repl->hdr.src_addr, 6);
	memcpy(frame_repl->hdr.src_addr,   frame_repl->hdr.dest_addr, 6);	
	frame_repl->data_len = dat_len; 
	enc28_packet_transmit(frame_repl);
}

void eth_request(enc28_frame *frame_req, uint16_t prot_type, uint8_t *mac_dest, uint16_t dat_len)
{
	memcpy(frame_req->hdr.dest_addr,   mac_dest, 6);
	memcpy(frame_req->hdr.src_addr,   mac_addr, 6);
	frame_req->hdr.type = prot_type;
	frame_req->data_len = dat_len; 
	enc28_packet_transmit(frame_req);
}


void arp_reply(enc28_frame *frame_rec,  uint16_t arp_pckt_len)
{
	arp_pckt *arp_repl = (void*)frame_rec->data;
	arp_repl->req_type = ARP_REP;
	memcpy(arp_repl->mac_dest, arp_repl->mac_src, 6);
	memcpy(arp_repl->mac_src, mac_addr, 6);
	memcpy(arp_repl->ip_dest, arp_repl->ip_src, 4);
	memcpy(arp_repl->ip_src, ip_addr, 4);
	
	eth_reply(frame_rec, sizeof(arp_pckt));
	
}

uint8_t arp_request (uint8_t *ip_dst)
{
	enc28_frame frame_req;
	frame_req.data = (uint8_t*) malloc(sizeof(arp_pckt) * sizeof(uint8_t));
	arp_pckt *arp_req = (void *)frame_req.data;
	
	arp_req->net_type = ARP_ETH;
	arp_req->prot_type = ARP_IP;
	arp_req->mac_len = 6;
	arp_req->ip_len = 4;
	arp_req->req_type = ARP_REQ;
	memcpy(arp_req->ip_src, ip_addr, 4);
	memcpy(arp_req->mac_src, mac_addr, 6);
	memcpy(arp_req->mac_dest, mac_addr_null, 6);
	memcpy(arp_req->ip_dest, ip_dst, 4);
	
	eth_request(&frame_req, ARP_TYPE, mac_addr_bc, sizeof(arp_pckt));
	free(frame_req.data);
	
	return 1;
}


void fill_arp_table(arp_record *arp_tbl_ptr, uint8_t *cur_len, uint8_t arp_tbl_len, uint32_t clk, enc28_frame *frame_rec)
{
	uint8_t i;
	arp_pckt *arp_repl = (arp_pckt *)frame_rec->data;
	
	if (*cur_len >= arp_tbl_len)
		*cur_len = arp_tbl_len - 1;
	for(i = 0; i < *cur_len; i++)
		if(!memcmp(arp_tbl_ptr[i].ip_addr, arp_repl->ip_src, 4))
			break;
	if(i == *cur_len)
	{		
		memcpy(arp_tbl_ptr[*cur_len].mac_addr, arp_repl->mac_src, 6);
		memcpy(arp_tbl_ptr[*cur_len].ip_addr, arp_repl->ip_src, 4);
		arp_tbl_ptr[*cur_len].clk = clk;
		(*cur_len)++;
	}
	for(i = 0; i < *cur_len; i++)
	{
		if((clk_s - arp_tbl_ptr[i].clk) > CLK_TIM_OUT)
			dlt_arp_table_record(arp_tbl_ptr, i, cur_len);
		
	}
}

int8_t find_arp_table_data(arp_record *arp_tbl_ptr, uint8_t cur_len, uint8_t *ip_num)
{
	int8_t i;
	
	for(i = 0; i < cur_len; i++)
		if(!memcmp(arp_tbl_ptr->ip_addr, ip_num, 4))
			break;
		
	if(i == cur_len)
		i = -1;
	
	return i;
	
}

void show_arp_table(arp_record *arp_tbl_ptr, uint8_t cur_len)
{
	uint8_t i;
	char s1[STR_LEN];
	
	uart_transmit((uint8_t *)"\r\n", 2);
	sprintf(s1,"ARP Table\r\n");
	uart_transmit((uint8_t *)s1, strlen(s1));
	
	for(i = 0; i < cur_len; i++)
	{
		sprintf(s1,"%d: mac_address: %02X:%02X:%02X:%02X:%02X:%02X - source_address: %d:%d:%d:%d, time: %d\r\n", i+1, arp_tbl_ptr[i].mac_addr[0],\
		arp_tbl_ptr[i].mac_addr[1], arp_tbl_ptr[i].mac_addr[2], arp_tbl_ptr[i].mac_addr[3], arp_tbl_ptr[i].mac_addr[4], arp_tbl_ptr[i].mac_addr[5],\
		arp_tbl_ptr[i].ip_addr[0], arp_tbl_ptr[i].ip_addr[1], arp_tbl_ptr[i].ip_addr[2], arp_tbl_ptr[i].ip_addr[3], arp_tbl_ptr[i].clk);
		uart_transmit((uint8_t *)s1, strlen(s1));
				
	}
}

void dlt_arp_table_record(arp_record *arp_tbl_ptr, uint8_t dlt_index, uint8_t *cur_len)
{
	uint8_t i;
	
	for(i = dlt_index; i <  (*cur_len - 1); i++)
		arp_tbl_ptr[i] = arp_tbl_ptr[i+1];
	
	(*cur_len)--;
	
}


uint8_t arp_read(enc28_frame *frame_rec, uint16_t len)
{
	uint8_t res = 0;
	char s1[STR_LEN];
	
	arp_pckt *arp_mess = (void *)(frame_rec->data);
	if(((arp_mess->net_type) == ARP_ETH) && (arp_mess->prot_type == ARP_IP) && (len >= sizeof(arp_pckt))&& (!memcmp(arp_mess->ip_dest, ip_addr, 4)))
	{
		if((arp_mess->req_type) == ARP_REQ)
		{

			sprintf(s1,"ARP request\r\n");
			uart_transmit((uint8_t *)s1, strlen(s1));
			res = 1;
			
		}
		else if ((arp_mess->req_type) == ARP_REP)
		{
			sprintf(s1,"ARP reply\r\n");
			uart_transmit((uint8_t *)s1, strlen(s1));
			res = 2;
			
		}
		
		sprintf(s1,"mac_source %02X:%02X:%02X:%02X:%02X:%02X\r\n", arp_mess->mac_src[0],\
		arp_mess->mac_src[1], arp_mess->mac_src[2], arp_mess->mac_src[3], arp_mess->mac_src[4], arp_mess->mac_src[5]);
		uart_transmit((uint8_t *)s1, strlen(s1));
			
		sprintf(s1,"ip_source %03d.%03d.%03d.%03d\r\n", arp_mess->ip_src[0],\
		arp_mess->ip_src[1], arp_mess->ip_src[2], arp_mess->ip_src[3]);
		uart_transmit((uint8_t *)s1, strlen(s1));
			
		sprintf(s1,"mac_dest %02X:%02X:%02X:%02X:%02X:%02X\r\n", arp_mess->mac_dest[0],\
		arp_mess->mac_dest[1], arp_mess->mac_dest[2], arp_mess->mac_dest[3], arp_mess->mac_dest[4], arp_mess->mac_dest[5]);
		uart_transmit((uint8_t *)s1, strlen(s1));
			
		sprintf(s1,"ip_dest %03d.%03d.%03d.%03d\r\n", arp_mess->ip_dest[0],\
		arp_mess->ip_dest[1], arp_mess->ip_dest[2], arp_mess->ip_dest[3]);
		uart_transmit((uint8_t *)s1, strlen(s1));
		
	}
		return res;
}


uint8_t ip_reply(enc28_frame *frame_repl, uint16_t ip_dat_len)
{
	ip_pckt ip_msg; 
	uint8_t res = 0;
	
	ip_msg.hdr = (void *)frame_repl->data;
	ip_msg.data_len = ip_dat_len;
	
	ip_msg.hdr->flgs_fr_off = 0;
	ip_msg.hdr->ttl = 128;
	memcpy(ip_msg.hdr->ip_dest, ip_msg.hdr->ip_src,4);
	memcpy(ip_msg.hdr->ip_src, ip_addr,4);
	ip_msg.hdr->pack_len =  be16toword(ip_msg.data_len + sizeof(ip_header));
	
	ip_msg.hdr->ch_sum = 0;
	ip_msg.hdr->ch_sum = ip_ch_sum_calc((void *)ip_msg.hdr, sizeof(ip_header), 0);
	
	
	eth_reply(frame_repl, ip_msg.data_len + sizeof(ip_header));
	
	return res;
}

void ip_request(enc28_frame *frame_req, uint8_t prot, uint8_t *mac_dest, uint8_t *ip_dest, uint16_t dat_len)
{
	ip_pckt ip_req;
	
	ip_req.data_len = dat_len;
	ip_req.data = frame_req->data + sizeof(ip_header);
	
	ip_req.hdr = (void*)(frame_req->data);
	
	ip_req.hdr->ver_hlen = IP_VERLEN;
	ip_req.hdr->tos = 0;
	ip_req.hdr->pack_len =  be16toword(ip_req.data_len + sizeof(ip_header));
	ip_req.hdr->id = 0;
	ip_req.hdr->flgs_fr_off = 0;
	ip_req.hdr->ttl = 128;
	ip_req.hdr->prot_id = prot;
	memcpy(ip_req.hdr->ip_dest, ip_dest, 4);
	memcpy(ip_req.hdr->ip_src, ip_addr, 4);
	
	ip_req.hdr->ch_sum = 0;
	ip_req.hdr->ch_sum = ip_ch_sum_calc((uint8_t *)ip_req.hdr, sizeof(ip_header), 0);
	
	eth_request(frame_req, IP_TYPE, mac_dest, ip_req.data_len + sizeof(ip_header));
	
}

uint8_t ip_read(enc28_frame *frame_rec, uint16_t ip_len)
{
	uint8_t res = 0;
	char s1[STR_LEN];
	

	ip_pckt ip_msg;
	ip_msg.hdr = (void *)frame_rec->data;
	ip_msg.data = frame_rec->data + sizeof(ip_header);

	if((ip_len > sizeof(ip_header)) && (ip_msg.hdr->ver_hlen == IP_VERLEN) && (!memcmp(ip_msg.hdr->ip_dest, ip_addr, 4)))
	{
		ip_msg.data_len = be16toword(ip_msg.hdr->pack_len) - sizeof(ip_header);
		
		sprintf(s1,"ip request\r\n");
		uart_transmit((uint8_t *)s1, strlen(s1));
		
		sprintf(s1, "ip addr: %d:%d:%d:%d->%d:%d:%d:%d\r\n", ip_msg.hdr->ip_src[0],
		ip_msg.hdr->ip_src[1], ip_msg.hdr->ip_src[2], ip_msg.hdr->ip_src[3],
		ip_msg.hdr->ip_dest[0], ip_msg.hdr->ip_dest[1], ip_msg.hdr->ip_dest[2], ip_msg.hdr->ip_dest[3]);
		uart_transmit((uint8_t *)s1, strlen(s1));
		
		sprintf(s1, "ip ch_sum %04X\r\n", ip_msg.hdr->ch_sum);
		uart_transmit((uint8_t *)s1, strlen(s1));
		
		ip_msg.hdr->ch_sum = 0;
		ip_msg.hdr->ch_sum = ip_ch_sum_calc((uint8_t *)frame_rec->data, sizeof(ip_header), 0);
		sprintf(s1, "ip ch_sum calc: %04X\r\n", ip_msg.hdr->ch_sum);
		uart_transmit((uint8_t *)s1, strlen(s1));
		
		if(ip_msg.hdr->prot_id == IP_TCP)
		{
			tcp_read(frame_rec, ip_msg.data_len);
		}
		else if (ip_msg.hdr->prot_id == IP_UDP)
		{
			if(udp_read(frame_rec, ip_msg.data_len))
				udp_reply(frame_rec);
			
		}
		else if (ip_msg.hdr->prot_id == IP_ICMP)
		{
			if(icmp_read(frame_rec, ip_msg.data_len))
				icmp_reply(frame_rec, ip_msg.data_len);
				
		}
		
		res = 1;
	}
	
	return res;
}

uint8_t icmp_read(enc28_frame *frame_rec, uint16_t icmp_pckt_len)
{
	uint8_t res=0;
	char s1[STR_LEN];
	ip_pckt ip_msg;
	icmp_pckt icmp_msg;
	
	ip_msg.hdr = (void *)frame_rec->data;
	ip_msg.data = frame_rec->data + sizeof(ip_header);
	
	icmp_msg.hdr = (void *)ip_msg.data;
	icmp_msg.data = ip_msg.data + sizeof(icmp_header);
	icmp_msg.data_len = icmp_pckt_len - sizeof(icmp_header);
	
	if((icmp_pckt_len >= sizeof(icmp_header)) && (icmp_msg.hdr->msg_tp == ICMP_REQ))
	{
		
		sprintf(s1,"icmp request\r\n");
		uart_transmit((uint8_t *)s1, strlen(s1));
		
		sprintf(s1,"icmp check_sum: %04X\r\n", icmp_msg.hdr->ch_sum);
		uart_transmit((uint8_t *)s1, strlen(s1));
		
		icmp_msg.hdr->ch_sum = 0;
		icmp_msg.hdr->ch_sum = ip_ch_sum_calc((uint8_t *)icmp_msg.hdr, icmp_pckt_len, 0);
		sprintf(s1,"icmp calc check_sum: %04X\r\n", icmp_msg.hdr->ch_sum);
		uart_transmit((uint8_t *)s1, strlen(s1));
		
		res = 1;
	}
	return res;
	
}

void icmp_reply(enc28_frame *frame_rec, uint16_t icmp_pckt_len)
{
	ip_pckt ip_msg;
	icmp_pckt icmp_msg;
	
	ip_msg.hdr = (void *)frame_rec->data;
	ip_msg.data = frame_rec->data + sizeof(ip_header);
	
	icmp_msg.hdr = (void *)ip_msg.data;
	icmp_msg.data = ip_msg.data + sizeof(icmp_header);
	icmp_msg.data_len = icmp_pckt_len - sizeof(icmp_header);
	
	icmp_msg.hdr->msg_tp = ICMP_REP;
	icmp_msg.hdr->ch_sum = 0;
	icmp_msg.hdr->ch_sum = ip_ch_sum_calc((uint8_t *)icmp_msg.hdr, icmp_pckt_len, 0);

	ip_reply(frame_rec, icmp_pckt_len);
	
}


uint8_t udp_read(enc28_frame *frame_rec, uint16_t udp_pckt_len)
{
	uint8_t res = 0;
	char s1[STR_LEN];
	ip_pckt ip_msg;
	udp_pckt udp_msg;
	
  ip_msg.hdr = (void *)frame_rec->data;
	ip_msg.data = frame_rec->data + sizeof(ip_header);
	
	udp_msg.hdr = (void *)ip_msg.data;
	udp_msg.data = ip_msg.data + sizeof(udp_header);
	udp_msg.data_len = udp_pckt_len - sizeof(udp_header);

	
	sprintf(s1,"udp request\r\n");
	uart_transmit((uint8_t *)s1, strlen(s1));
	
	sprintf(s1,"udp source port: %u \r\n", be16toword(udp_msg.hdr->src_prt));
	uart_transmit((uint8_t *)s1, strlen(s1));
	
	sprintf(s1,"udp dest port: %u \r\n", be16toword(udp_msg.hdr->dst_prt));
	uart_transmit((uint8_t *)s1, strlen(s1));
	
	sprintf(s1,"udp check_sum: %04X\r\n", udp_msg.hdr->ch_sum);
	uart_transmit((uint8_t *)s1, strlen(s1));
	
	udp_msg.hdr->ch_sum = 0;
	udp_msg.hdr->ch_sum = ip_ch_sum_calc(((uint8_t *)udp_msg.hdr)-8, udp_pckt_len + 8, UDP_CH_SUM);
	
	sprintf(s1,"udp check_sum calc: %04X\r\n", udp_msg.hdr->ch_sum);
	uart_transmit((uint8_t *)s1, strlen(s1));
	
	strncpy(s1, (const char *)udp_msg.data, udp_msg.data_len);
	s1[udp_msg.data_len] = 0;
	uart_transmit((uint8_t *)s1, strlen(s1));
	uart_transmit((uint8_t *)"\r\n", 2);
	
	res = 1;
	
	return res;
}

void udp_reply(enc28_frame *frame_rec)
{
	enc28_frame frame_repl;
	ip_pckt ip_msg;
	udp_pckt udp_repl;
	uint16_t port;
	char s1[STR_LEN];
	
	ip_msg.hdr = (void *)frame_rec->data;
	ip_msg.data = frame_rec->data + sizeof(ip_header);
	
	udp_repl.hdr = (void *)ip_msg.data;
	
	memcpy((void *)&frame_repl.hdr, (void *)&(frame_rec->hdr), sizeof(enc28_frame_header)); 
	
	sprintf(s1,"Hello from UDP-Server to UDP-Client!!!\r\n");
	frame_repl.data = (uint8_t*) malloc((sizeof(ip_header) + sizeof(udp_header) + strlen(s1)) * sizeof(uint8_t));
	memcpy(frame_repl.data, (void*)ip_msg.hdr, sizeof(ip_header));
	memcpy(frame_repl.data + sizeof(ip_header), (void*)udp_repl.hdr, sizeof(udp_header));
	memcpy(frame_repl.data + sizeof(ip_header) + sizeof(udp_header), (void*)s1, strlen(s1));
	
	udp_repl.hdr = (void *)(frame_repl.data + sizeof(ip_header));
	udp_repl.data = frame_repl.data + sizeof(ip_header) + sizeof(udp_header);
	udp_repl.data_len = strlen(s1);
	
	port = udp_repl.hdr->dst_prt;
	udp_repl.hdr->dst_prt = udp_repl.hdr->src_prt;
	udp_repl.hdr->src_prt = port;
	udp_repl.hdr->pck_len = be16toword(udp_repl.data_len + sizeof(udp_header));
	udp_repl.hdr->ch_sum = 0;
	udp_repl.hdr->ch_sum = ip_ch_sum_calc(frame_repl.data + sizeof(ip_header) - 8, udp_repl.data_len + sizeof(udp_header) + 8, UDP_CH_SUM);
	
	ip_reply(&frame_repl, udp_repl.data_len + sizeof(udp_header));
	
	free(frame_repl.data);
}

void udp_request(uint8_t *mac_dest, uint8_t *ip_dest, uint16_t port_dest, uint8_t *data, uint16_t dat_len)
{
	udp_pckt udp_req;
	enc28_frame frame_req;
	
	frame_req.data = (uint8_t*) malloc((sizeof(ip_header) + sizeof(udp_header) + dat_len) * sizeof(uint8_t));
	
	udp_req.data_len = dat_len;
	udp_req.data = frame_req.data + sizeof(ip_header) + sizeof(udp_header);
	udp_req.hdr = (void*)(udp_req.data - sizeof(udp_header));
	
	memcpy((void*)(udp_req.data - sizeof(udp_header) - 8), ip_addr, 4);
	memcpy((void*)(udp_req.data - sizeof(udp_header) - 4), ip_dest, 4);
	memcpy(udp_req.data, data, dat_len);
	
	udp_req.hdr->dst_prt = be16toword(port_dest);
	udp_req.hdr->src_prt = be16toword(UDP_PORT_SRC);
	udp_req.hdr->pck_len = be16toword(udp_req.data_len + sizeof(udp_header));
	udp_req.hdr->ch_sum = 0;
	udp_req.hdr->ch_sum = ip_ch_sum_calc((void*)(udp_req.data - sizeof(udp_header)-8), udp_req.data_len + sizeof(udp_header) + 8, UDP_CH_SUM);
	
	
	ip_request(&frame_req, IP_UDP, mac_dest, ip_dest, udp_req.data_len + sizeof(udp_header));
	
	free(frame_req.data);
}

uint8_t tcp_read(enc28_frame *frame_rec, uint16_t tcp_pckt_len)
{
	uint8_t res = 0;
	uint16_t i;
	char s1[STR_LEN];
	ip_pckt ip_msg;
	tcp_pckt tcp_msg;
	
	ip_msg.hdr = (void *)frame_rec->data;
	ip_msg.data = frame_rec->data + sizeof(ip_header);
	
	tcp_msg.hdr = (void *)ip_msg.data;
	tcp_msg.data = ip_msg.data + sizeof(tcp_header);
	tcp_msg.data_len = tcp_pckt_len - sizeof(tcp_header);
	
	sprintf(s1,"TCP request\r\n");
	uart_transmit((uint8_t *)s1, strlen(s1));
	
	if(tcp_msg.hdr->flgs == (TCP_PSH | TCP_ACK))
	{
		if(tcp_msg.data_len)
		{
			for(i = 0; i < tcp_msg.data_len; i++)
				uart_transmit((uint8_t *)&tcp_msg.data[i], 1);
			uart_transmit((uint8_t *)"\r\n", 2);
			
			tcp_reply(frame_rec, TCP_OP_ACK_OF_DATA, tcp_pckt_len);
			
		}
		else
			tcp_reply(frame_rec, TCP_OP_ACK_OF_FIN, tcp_pckt_len);
	}
	
	else if(tcp_msg.hdr->flgs == TCP_SYN)
	{
		tcp_reply(frame_rec, TCP_OP_SYNACK, tcp_pckt_len);
	}
	else if (tcp_msg.hdr->flgs == TCP_ACK)
	{
		sprintf(s1,"ACK received\r\n");
		uart_transmit((uint8_t *)s1, strlen(s1));

			
	}
	else if (tcp_msg.hdr->flgs == (TCP_FIN | TCP_ACK))
	{
		tcp_reply(frame_rec, TCP_OP_ACK_OF_FIN, tcp_pckt_len);
	}
	res = 1;
	
	return res;
	
}

void tcp_reply(enc28_frame *frame_rec, uint8_t op_type, uint16_t tcp_pckt_len)
{
	static uint8_t tcp_data_rec = 0;
	uint32_t num_seg;
	uint16_t port, tcp_data_rec_len;
	char s1[STR_LEN];
	enc28_frame frame_repl;
	ip_pckt ip_msg;
	tcp_pckt tcp_repl;
	
	ip_msg.hdr = (void *)frame_rec->data;
	ip_msg.data = frame_rec->data + sizeof(ip_header);
	
	tcp_repl.hdr = (void *)ip_msg.data;
	
	memcpy((void *)&frame_repl.hdr, (void *)&(frame_rec->hdr), sizeof(enc28_frame_header)); 
	
	tcp_repl.data_len = 0;
	tcp_repl.data = NULL;
	
	if(op_type == TCP_OP_SYNACK)
		tcp_repl.data_len = 4;
	
	frame_repl.data = (uint8_t*) malloc((sizeof(ip_header) + sizeof(tcp_header) + tcp_repl.data_len) * sizeof(uint8_t));
	memcpy(frame_repl.data, (void*)ip_msg.hdr, sizeof(ip_header));
	memcpy(frame_repl.data + sizeof(ip_header), (void*)tcp_repl.hdr, sizeof(tcp_header));
	tcp_repl.hdr = (void*)(frame_repl.data + sizeof(ip_header));
	
	if(op_type == TCP_OP_SYNACK)
	{
		
		port = tcp_repl.hdr->src_prt;
		tcp_repl.hdr->src_prt = tcp_repl.hdr->dst_prt;
		tcp_repl.hdr->dst_prt = port;
		tcp_repl.hdr->ack_sn = be32todword(be32todword(tcp_repl.hdr->sn)+1);
		tcp_repl.hdr->sn = 1;
		tcp_repl.hdr->flgs = TCP_SYN | TCP_ACK;
		tcp_repl.hdr->hdr_len = (uint8_t)((sizeof(tcp_header) + tcp_repl.data_len) << 2);
		tcp_repl.hdr->win_size = be16toword(TCP_WIN_LEN);
		tcp_repl.hdr->urg_ptr = 0;
		
		tcp_repl.data = frame_repl.data + sizeof(ip_header) + sizeof(tcp_header);
		tcp_repl.data[0] = 2;
		tcp_repl.data[1] = 4;
		tcp_repl.data[2] = (uint8_t)(TCP_SEG_LEN >> 8);
		tcp_repl.data[3] = (uint8_t)TCP_SEG_LEN;
		
		tcp_repl.hdr->ch_sum = 0;
		tcp_repl.hdr->ch_sum = ip_ch_sum_calc(frame_repl.data + sizeof(ip_header) - 8, tcp_repl.data_len + sizeof(tcp_header) + 8, TCP_CH_SUM);
	
		ip_reply(&frame_repl, tcp_repl.data_len + sizeof(tcp_header));
		sprintf(s1,"SYN ACK transmitted\r\n");
		uart_transmit((uint8_t *)s1, strlen(s1));
		
	}
	else if(op_type == TCP_OP_ACK_OF_FIN)
	{
		
		port = tcp_repl.hdr->src_prt;
		tcp_repl.hdr->src_prt = tcp_repl.hdr->dst_prt;
		tcp_repl.hdr->dst_prt = port;
		num_seg = be32todword(tcp_repl.hdr->ack_sn); 
		tcp_repl.hdr->ack_sn = be32todword(be32todword(tcp_repl.hdr->sn)+1);
		tcp_repl.hdr->sn = be32todword(num_seg);
		tcp_repl.hdr->flgs = TCP_ACK;
		tcp_repl.hdr->hdr_len = (uint8_t)((sizeof(tcp_header) + tcp_repl.data_len) << 2);
		tcp_repl.hdr->win_size = be16toword(TCP_WIN_LEN);
		tcp_repl.hdr->urg_ptr = 0;
		tcp_repl.hdr->ch_sum = 0;
		tcp_repl.hdr->ch_sum = ip_ch_sum_calc(frame_repl.data + sizeof(ip_header) - 8, tcp_repl.data_len + sizeof(tcp_header) + 8, TCP_CH_SUM);
		
		ip_reply(&frame_repl, tcp_repl.data_len + sizeof(tcp_header));
		sprintf(s1,"ACK transmitted\r\n");
		uart_transmit((uint8_t *)s1, strlen(s1));
		
		memcpy((void *)&frame_repl.hdr, (void *)&(frame_rec->hdr), sizeof(enc28_frame_header)); 
		memcpy(frame_repl.data, (void*)ip_msg.hdr, sizeof(ip_header));
		tcp_repl.hdr->flgs = TCP_FIN | TCP_ACK;
		tcp_repl.hdr->ch_sum = 0;
		tcp_repl.hdr->ch_sum = ip_ch_sum_calc(frame_repl.data + sizeof(ip_header) - 8, tcp_repl.data_len + sizeof(tcp_header) + 8, TCP_CH_SUM);
		ip_reply(&frame_repl, tcp_repl.data_len + sizeof(tcp_header));
		
		sprintf(s1,"FIN ACK transmitted\r\n");
		uart_transmit((uint8_t *)s1, strlen(s1));
	}
	else if(op_type == TCP_OP_ACK_OF_DATA)
	{
		tcp_data_rec++;
		tcp_data_rec_len = tcp_pckt_len - sizeof(tcp_header);
		port = tcp_repl.hdr->src_prt;
		tcp_repl.hdr->src_prt = tcp_repl.hdr->dst_prt;
		tcp_repl.hdr->dst_prt = port;
		num_seg = be32todword(tcp_repl.hdr->ack_sn); 
		tcp_repl.hdr->ack_sn = be32todword(be32todword(tcp_repl.hdr->sn)+tcp_data_rec_len);
		tcp_repl.hdr->sn = be32todword(num_seg);
		tcp_repl.hdr->flgs = TCP_ACK;
		tcp_repl.hdr->hdr_len = (uint8_t)((sizeof(tcp_header)) << 2);
		tcp_repl.hdr->win_size = be16toword(TCP_WIN_LEN);
		tcp_repl.hdr->urg_ptr = 0;
		tcp_repl.hdr->ch_sum = 0;
		tcp_repl.hdr->ch_sum = ip_ch_sum_calc(frame_repl.data + sizeof(ip_header) - 8, sizeof(tcp_header) + 8, TCP_CH_SUM);
	
		ip_reply(&frame_repl, sizeof(tcp_header));
		sprintf(s1,"DATA ACK transmitted\r\n");
		uart_transmit((uint8_t *)s1, strlen(s1));
		if(tcp_data_rec == 2)
		{
			tcp_data_rec = 0;
			free(frame_repl.data);
			
			sprintf(s1,"Hello from TCP Server to TCP Client\r\n");
			tcp_repl.data_len = strlen(s1);
			
			tcp_repl.hdr = (void *)ip_msg.data;
			memcpy((void *)&frame_repl.hdr, (void *)&(frame_rec->hdr), sizeof(enc28_frame_header)); 
			
			frame_repl.data = (uint8_t*) malloc((sizeof(ip_header) + sizeof(tcp_header) + tcp_repl.data_len) * sizeof(uint8_t));
			memcpy(frame_repl.data, (void*)ip_msg.hdr, sizeof(ip_header));
			memcpy(frame_repl.data + sizeof(ip_header), (void*)tcp_repl.hdr, sizeof(tcp_header));
			memcpy(frame_repl.data + sizeof(ip_header) + sizeof(tcp_header), (void*)s1, tcp_repl.data_len);
			tcp_repl.hdr = (void*)(frame_repl.data + sizeof(ip_header));
			tcp_repl.data = frame_repl.data + sizeof(ip_header) +  sizeof(tcp_header);
			
			port = tcp_repl.hdr->src_prt;
			tcp_repl.hdr->src_prt = tcp_repl.hdr->dst_prt;
			tcp_repl.hdr->dst_prt = port;
			num_seg = be32todword(tcp_repl.hdr->ack_sn); 
			tcp_repl.hdr->ack_sn = be32todword(be32todword(tcp_repl.hdr->sn)+tcp_data_rec_len);
			tcp_repl.hdr->sn = be32todword(num_seg);
			tcp_repl.hdr->flgs = TCP_PSH | TCP_ACK;
			tcp_repl.hdr->hdr_len = (uint8_t)((sizeof(tcp_header)) << 2);
			tcp_repl.hdr->win_size = be16toword(TCP_WIN_LEN);
			tcp_repl.hdr->urg_ptr = 0;
			tcp_repl.hdr->ch_sum = 0;
			tcp_repl.data_len = strlen(s1);
			
			tcp_repl.hdr->ch_sum = ip_ch_sum_calc(frame_repl.data + sizeof(ip_header) - 8, sizeof(tcp_header) + tcp_repl.data_len + 8, TCP_CH_SUM);
			ip_reply(&frame_repl, tcp_repl.data_len + sizeof(tcp_header));
			sprintf(s1,"DATA transmitted\r\n");
			uart_transmit((uint8_t *)s1, strlen(s1));
		}
	}
	
	free(frame_repl.data);
	
}

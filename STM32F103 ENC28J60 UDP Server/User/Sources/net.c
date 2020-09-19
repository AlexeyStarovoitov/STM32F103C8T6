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
extern uint8_t ip_serv[4];
extern enc28_frame frame;
extern uint32_t clk_s; 

arp_record arp_table[ARP_TABLE_LEN];
uint8_t cur_index;
uart_ip ip_serv_str;

void net_init(void)
{
	frame.data = NULL;
	frame.data_len = 0;
	
	ip_serv_str.buf_len = 0;
	ip_serv_str.ip_rec = 0;
	
	cur_index = 0;
	clk_s = 0;
	uart_transmit((uint8_t *)"Start\r\n", 7);
}

void ip_str2num(char *ip_str, uint8_t *ip_num)
{
	uint8_t i, prt_len;
	char prt_str[5];
	char *dot_ptr;
	
	for(i = 0; i < 4; i++)
	{
		if(i != 3)
			dot_ptr = strchr(ip_str, '.');
		else
			dot_ptr = strchr(ip_str, '\0');
		
		prt_len = dot_ptr - ip_str+1;
		strncpy(prt_str, ip_str, prt_len);
		prt_str[prt_len] = 0;
		ip_num[i] = atoi(prt_str);
		ip_str = dot_ptr+1;
		
	}

}

uint16_t ip_ch_sum_calc(uint8_t *head, uint16_t len, uint8_t udp_type)
{
	
	uint32_t sum = 0;
	
	if(udp_type)
	{
		sum += IP_UDP;
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

void eth_send(enc28_frame *frame_rec, uint8_t *data, uint16_t dat_len)
{
	enc28_frame frame_repl;
	memcpy(frame_repl.hdr.dest_addr,   frame_rec->hdr.src_addr, 6);
	memcpy(frame_repl.hdr.src_addr,   frame_rec->hdr.dest_addr, 6);
	frame_repl.hdr.type = frame_rec->hdr.type;
	frame_repl.data_len = dat_len; 
	frame_repl.data = data;
	enc28_packet_transmit(&frame_repl);
}


void arp_send(enc28_frame *frame_rec,  uint16_t arp_pckt_len)
{
	
	arp_pckt arp_repl;
	memcpy((void*)&arp_repl, frame_rec->data, sizeof(arp_pckt));
	arp_repl.req_type = ARP_REP;
	memcpy(arp_repl.mac_dest, arp_repl.mac_src, 6);
	memcpy(arp_repl.mac_src, mac_addr, 6);
	memcpy(arp_repl.ip_dest, arp_repl.ip_src, 4);
	memcpy(arp_repl.ip_src, ip_addr, 4);
	
	eth_send(frame_rec, (uint8_t *)&arp_repl, sizeof(arp_pckt));
	
}

uint8_t arp_request (uint8_t *ip_dst)
{
	enc28_frame frame_snd;
	
	frame_snd.data_len = sizeof(arp_pckt);
	frame_snd.data = NULL;
	arp_pckt arp_req;
	
	arp_req.net_type = ARP_ETH;
	arp_req.prot_type = ARP_IP;
	arp_req.mac_len = 6;
	arp_req.ip_len = 4;
	arp_req.req_type = ARP_REQ;
	memcpy(arp_req.ip_src, ip_addr, 4);
	memcpy(arp_req.mac_src, mac_addr, 6);
	memcpy(arp_req.mac_dest, mac_addr_null, 6);
	memcpy(arp_req.ip_dest, ip_dst, 4);
	
	memcpy(frame_snd.hdr.dest_addr, mac_addr, 6);
	memcpy(frame_snd.hdr.src_addr, mac_addr_bc, 6);
	frame_snd.hdr.type = ARP_TYPE;
	
	eth_send(&frame_snd, (uint8_t *)&arp_req, sizeof(arp_pckt));

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


uint8_t ip_send(enc28_frame *frame_rec, uint8_t *data, uint16_t ip_dat_len)
{
	ip_pckt ip_msg; 
	uint8_t res = 0;
	
	ip_msg.hdr = (void *)frame_rec->data;
	ip_msg.pckt_len = ip_dat_len + sizeof(ip_header);
	ip_msg.pckt = (uint8_t *) malloc(ip_msg.pckt_len * sizeof(uint8_t));
	
	ip_msg.hdr->flgs_fr_off = 0;
	ip_msg.hdr->ttl = 128;
	memcpy(ip_msg.hdr->ip_dest, ip_msg.hdr->ip_src,4);
	memcpy(ip_msg.hdr->ip_src, ip_addr,4);
	ip_msg.hdr->pack_len =  be16toword(ip_msg.pckt_len);
	
	ip_msg.hdr->ch_sum = 0;
	ip_msg.hdr->ch_sum = ip_ch_sum_calc((void *)ip_msg.hdr, sizeof(ip_header), 0);
	
	memcpy(ip_msg.pckt, (void *)ip_msg.hdr, sizeof(ip_header));
	memcpy(ip_msg.pckt + sizeof(ip_header), data, ip_dat_len);
	eth_send(frame_rec, ip_msg.pckt, ip_msg.pckt_len);
	
	free(ip_msg.pckt);
	return res;
}

uint8_t ip_read(enc28_frame *frame_rec, uint16_t ip_len)
{
	uint8_t res = 0;
	char s1[STR_LEN];
	

	ip_pckt ip_msg;
	ip_msg.hdr = (void *)frame_rec->data;
	ip_msg.pckt = frame_rec->data;

	if((ip_len > sizeof(ip_header)) && (ip_msg.hdr->ver_hlen == IP_VERLEN) && (!memcmp(ip_msg.hdr->ip_dest, ip_addr, 4)))
	{
		ip_msg.pckt_len = be16toword(ip_msg.hdr->pack_len);
//		ip_msg.pckt_len = ip_len;
		
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
		}
		else if (ip_msg.hdr->prot_id == IP_UDP)
		{
			udp_read(frame_rec, ip_msg.pckt_len - sizeof(ip_header));
			
		}
		else if (ip_msg.hdr->prot_id == IP_ICMP)
		{
			icmp_read(frame_rec, ip_msg.pckt_len - sizeof(ip_header));
				
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
	icmp_pckt icmp_msg, icmp_repl;
	
	ip_msg.hdr = (void *)frame_rec->data;
	ip_msg.pckt = frame_rec->data;
	
	icmp_msg.hdr = (void *)(ip_msg.pckt + sizeof(ip_header));
	icmp_msg.pckt = ip_msg.pckt + sizeof(ip_header);
	icmp_msg.pckt_len = icmp_pckt_len;
	
	if((icmp_pckt_len >= sizeof(icmp_header)) && (icmp_msg.hdr->msg_tp == ICMP_REQ))
	{
		
		sprintf(s1,"icmp request\r\n");
		uart_transmit((uint8_t *)s1, strlen(s1));
		
		sprintf(s1,"icmp check_sum: %04X\r\n", icmp_msg.hdr->ch_sum);
		uart_transmit((uint8_t *)s1, strlen(s1));
		
		icmp_msg.hdr->ch_sum = 0;
		icmp_msg.hdr->ch_sum = ip_ch_sum_calc(icmp_msg.pckt, icmp_pckt_len, 0);
		sprintf(s1,"icmp calc check_sum: %04X\r\n", icmp_msg.hdr->ch_sum);
		uart_transmit((uint8_t *)s1, strlen(s1));
		
		icmp_msg.hdr->msg_tp = ICMP_REP;
		icmp_msg.hdr->ch_sum = 0;
		icmp_msg.hdr->ch_sum = ip_ch_sum_calc(icmp_msg.pckt, icmp_pckt_len, 0);
		
		icmp_repl.pckt = (uint8_t *) malloc(icmp_pckt_len * sizeof(uint8_t));
		memcpy(icmp_repl.pckt, (void *)icmp_msg.hdr, sizeof(icmp_header));
		memcpy(icmp_repl.pckt + sizeof(icmp_header), icmp_msg.pckt + sizeof(icmp_header), icmp_pckt_len - sizeof(icmp_header));
		
		ip_send(frame_rec, icmp_repl.pckt, icmp_pckt_len);
		free(icmp_repl.pckt);
		res = 1;
	}
	return res;
	
}


uint8_t udp_read(enc28_frame *frame_rec, uint16_t udp_pckt_len)
{
	uint8_t res = 0;
	char s1[STR_LEN];
	ip_pckt ip_msg;
	udp_pckt udp_msg;
	
  ip_msg.hdr = (void *)frame_rec->data;
	ip_msg.pckt = frame_rec->data;
	
	udp_msg.hdr = (void *)(ip_msg.pckt + sizeof(ip_header));
	udp_msg.pckt = ip_msg.pckt + sizeof(ip_header);
	udp_msg.pckt_len = udp_pckt_len;

	
	sprintf(s1,"udp request\r\n");
	uart_transmit((uint8_t *)s1, strlen(s1));
	
	sprintf(s1,"udp source port: %u \r\n", be16toword(udp_msg.hdr->src_prt));
	uart_transmit((uint8_t *)s1, strlen(s1));
	
	sprintf(s1,"udp dest port: %u \r\n", be16toword(udp_msg.hdr->dst_prt));
	uart_transmit((uint8_t *)s1, strlen(s1));
	
	sprintf(s1,"udp check_sum: %04X\r\n", udp_msg.hdr->ch_sum);
	uart_transmit((uint8_t *)s1, strlen(s1));
	
	udp_msg.hdr->ch_sum = 0;
	udp_msg.hdr->ch_sum = ip_ch_sum_calc(((uint8_t *)udp_msg.hdr)-8, udp_pckt_len + 8, 1);
	
	sprintf(s1,"udp check_sum calc: %04X\r\n", udp_msg.hdr->ch_sum);
	uart_transmit((uint8_t *)s1, strlen(s1));
	
	strncpy(s1, (const char *)udp_msg.pckt+sizeof(udp_header), udp_msg.pckt_len - sizeof(udp_header));
	s1[udp_msg.pckt_len - sizeof(udp_header)] = 0;
	uart_transmit((uint8_t *)s1, strlen(s1));
	uart_transmit((uint8_t *)"\r\n", 2);
	
	udp_send(frame_rec);
	
	res = 1;
	
	return res;
}

void udp_send(enc28_frame *frame_rec)
{
	ip_pckt ip_msg;
	udp_pckt udp_repl;
	uint16_t port;
	char s1[STR_LEN];
	
	ip_msg.hdr = (void *)frame_rec->data;
	ip_msg.pckt = frame_rec->data;
	
	udp_repl.hdr = (void *)(ip_msg.pckt + sizeof(ip_header));
	
	port = udp_repl.hdr->dst_prt;
	udp_repl.hdr->dst_prt = udp_repl.hdr->src_prt;
	udp_repl.hdr->src_prt = port;
	udp_repl.hdr->ch_sum = 0;
	
	sprintf(s1,"Hello from UDP-Server to UDP-Client!!!\r\n");
	udp_repl.pckt_len = strlen(s1) + sizeof(udp_header);
	udp_repl.hdr->pck_len = be16toword(udp_repl.pckt_len);
	
	udp_repl.pckt = (uint8_t*) malloc((udp_repl.pckt_len + 8) * sizeof(uint8_t));
	memcpy(udp_repl.pckt, ip_msg.hdr->ip_src, 4);
	memcpy(udp_repl.pckt + 4, ip_msg.hdr->ip_dest, 4);
	memcpy(udp_repl.pckt + 8, (void*)udp_repl.hdr, sizeof(udp_header));
	strcpy((char *)(udp_repl.pckt + 8 + sizeof(udp_header)), s1);
	udp_repl.hdr = (void*)(udp_repl.pckt + 8);
	udp_repl.hdr->ch_sum = ip_ch_sum_calc(udp_repl.pckt, udp_repl.pckt_len + 8, 1);
	
	ip_send(frame_rec, udp_repl.pckt+8, udp_repl.pckt_len);
	
	free(udp_repl.pckt);
}


void net_poll(void)
{
	char s1[STR_LEN];
	uint16_t len;
	uint8_t res = 0;
	
	len = enc28_packet_receive(&frame);
	
	if(len > 0)
		{
			
			if(frame.hdr.type == ARP_TYPE)
				{
					uart_transmit((uint8_t *)"\r\n", 2);	
					sprintf(s1,"ethernet request\r\n");
					uart_transmit((uint8_t *)s1, strlen(s1));
		
					sprintf(s1,"MAC: %02X:%02X:%02X:%02X:%02X:%02X->%02X:%02X:%02X:%02X:%02X:%02X; len: %d prot: arp \r\n",\
					frame.hdr.src_addr[0], frame.hdr.src_addr[1], frame.hdr.src_addr[2], frame.hdr.src_addr[3],\
					frame.hdr.src_addr[4], frame.hdr.src_addr[5],\
					frame.hdr.dest_addr[0], frame.hdr.dest_addr[1], frame.hdr.dest_addr[2], frame.hdr.dest_addr[3],\
					frame.hdr.dest_addr[4], frame.hdr.dest_addr[5], len);
					uart_transmit((uint8_t *)s1, strlen(s1));
					
					res = arp_read(&frame, frame.data_len);
					if(res == 1)
						arp_send(&frame, frame.data_len);
					else if (res == 2)
					{
						fill_arp_table(arp_table, &cur_index, ARP_TABLE_LEN, clk_s, &frame);
						show_arp_table(arp_table, cur_index);
						
					}
				}
				else if(frame.hdr.type == IP_TYPE)
				{
					uart_transmit((uint8_t *)"\r\n", 2);	
					sprintf(s1,"MAC: %02X:%02X:%02X:%02X:%02X:%02X->%02X:%02X:%02X:%02X:%02X:%02X; len: %d prot: ip \r\n",\
					frame.hdr.src_addr[0], frame.hdr.src_addr[1], frame.hdr.src_addr[2], frame.hdr.src_addr[3],\
					frame.hdr.src_addr[4], frame.hdr.src_addr[5],\
					frame.hdr.dest_addr[0], frame.hdr.dest_addr[1], frame.hdr.dest_addr[2], frame.hdr.dest_addr[3],\
					frame.hdr.dest_addr[4], frame.hdr.dest_addr[5], len);
					uart_transmit((uint8_t *)s1, strlen(s1));
					ip_read(&frame, frame.data_len);
					
				}
			}
		if(frame.data)
		{
				free(frame.data);
				frame.data = NULL;
				frame.data_len = 0;
		}
	
}




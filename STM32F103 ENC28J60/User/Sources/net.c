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
extern uint8_t ip_addr[4];
extern enc28_frame frame;

void net_init(void)
{
	frame.data = NULL;
	frame.data_len = 0;
	uart_transmit((uint8_t *)"Start\r\n", 7);
}

uint16_t ip_ch_sum_calc(uint8_t *head, uint16_t len)
{
	uint32_t sum = 0;
	
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

void eth_send(enc28_frame *frame_rec, uint16_t dat_len)
{
	enc28_frame frame_repl;
	memcpy(frame_repl.hdr.dest_addr,   frame_rec->hdr.src_addr, 6);
	memcpy(frame_repl.hdr.src_addr,   frame_rec->hdr.dest_addr, 6);
	frame_repl.hdr.type = frame_rec->hdr.type;
	frame_repl.data_len = dat_len; 
	frame_repl.data = (uint8_t *) malloc((dat_len) * sizeof(uint8_t));
	memcpy(frame_repl.data,   frame_rec->data, dat_len);
	enc28_packet_transmit(&frame_repl);
	free(frame_repl.data);
}


void arp_send(enc28_frame *frame_rec,  uint16_t arp_pckt_len)
{
	
	arp_pckt *arp_repl = (void *)frame_rec->data;
	arp_repl->req_type = ARP_REP;
	memcpy(arp_repl->mac_dest, arp_repl->mac_src, 6);
	memcpy(arp_repl->mac_src, mac_addr, 6);
	memcpy(arp_repl->ip_dest, arp_repl->ip_src, 4);
	memcpy(arp_repl->ip_src, ip_addr, 4);
	
	eth_send(frame_rec, arp_pckt_len);
	
}

uint8_t arp_read(enc28_frame *frame_rec, uint16_t len)
{
	uint8_t res = 0;
	char s1[STR_LEN];
	
	arp_pckt *arp_mess = (void *)(frame_rec->data);
	if(((arp_mess->net_type) == ARP_ETH) && (arp_mess->prot_type == ARP_IP) && (len >= sizeof(arp_pckt)))
		if(((arp_mess->req_type) == ARP_REQ) && (!memcmp(arp_mess->ip_dest, ip_addr, 4)))
		{

			sprintf(s1,"ARP request\r\n");
			uart_transmit((uint8_t *)s1, strlen(s1));
			
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
			res = 1;
			
		}
		return res;
}


uint8_t ip_send(enc28_frame *frame_rec, uint16_t ip_dat_len)
{
	ip_pckt ip_msg; 
	uint8_t res = 0;
	ip_msg.hdr = (void *)frame_rec->data;
	ip_msg.data = frame_rec->data + sizeof(ip_header);
	
	
	ip_msg.hdr->flgs_fr_off = 0;
	ip_msg.hdr->ttl = 128;
	memcpy(ip_msg.hdr->ip_dest, ip_msg.hdr->ip_src,4);
	memcpy(ip_msg.hdr->ip_src, ip_addr,4);
	ip_msg.hdr->pack_len =  be16toword(ip_dat_len + sizeof(ip_header));
	
	ip_msg.hdr->ch_sum = 0;
	ip_msg.hdr->ch_sum = ip_ch_sum_calc((uint8_t *)frame_rec->data, sizeof(ip_header));
	eth_send(frame_rec, ip_dat_len + sizeof(ip_header));
	
	return res;
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
		ip_msg.dat_len = be16toword(ip_msg.hdr->pack_len) - sizeof(ip_header);
		
		sprintf(s1,"ip request\r\n");
		uart_transmit((uint8_t *)s1, strlen(s1));
		
		sprintf(s1, "ip addr: %d:%d:%d:%d->%d:%d:%d:%d\r\n", ip_msg.hdr->ip_src[0],
		ip_msg.hdr->ip_src[1], ip_msg.hdr->ip_src[2], ip_msg.hdr->ip_src[3],
		ip_msg.hdr->ip_dest[0], ip_msg.hdr->ip_dest[1], ip_msg.hdr->ip_dest[2], ip_msg.hdr->ip_dest[3]);
		uart_transmit((uint8_t *)s1, strlen(s1));
		
		sprintf(s1, "ip ch_sum %04X\r\n", ip_msg.hdr->ch_sum);
		uart_transmit((uint8_t *)s1, strlen(s1));
		
		ip_msg.hdr->ch_sum = 0;
		ip_msg.hdr->ch_sum = ip_ch_sum_calc((uint8_t *)frame_rec->data, sizeof(ip_header));
		sprintf(s1, "ip ch_sum calc: %04X\r\n", ip_msg.hdr->ch_sum);
		uart_transmit((uint8_t *)s1, strlen(s1));
		
		if(ip_msg.hdr->prot_id == IP_TCP)
		{
		}
		else if (ip_msg.hdr->prot_id == IP_UDP)
		{
			
		}
		else if (ip_msg.hdr->prot_id == IP_ICMP)
		{
			icmp_read(frame_rec, ip_msg.dat_len);
				
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
	icmp_msg.dat_len = icmp_pckt_len - sizeof(icmp_header);
	
	if((icmp_pckt_len >= sizeof(icmp_header)) && (icmp_msg.hdr->msg_tp == ICMP_REQ))
	{
		
		sprintf(s1,"icmp request\r\n");
		uart_transmit((uint8_t *)s1, strlen(s1));
		
		sprintf(s1,"icmp check_sum: %04X\r\n", icmp_msg.hdr->ch_sum);
		uart_transmit((uint8_t *)s1, strlen(s1));
		
		icmp_msg.hdr->ch_sum = 0;
		icmp_msg.hdr->ch_sum = ip_ch_sum_calc((uint8_t *)ip_msg.data, icmp_pckt_len);
		sprintf(s1,"icmp calc check_sum: %04X\r\n", icmp_msg.hdr->ch_sum);
		uart_transmit((uint8_t *)s1, strlen(s1));
		
		icmp_msg.hdr->msg_tp = ICMP_REP;
		icmp_msg.hdr->ch_sum = 0;
		icmp_msg.hdr->ch_sum = ip_ch_sum_calc((uint8_t *)ip_msg.data, icmp_pckt_len);
		
		
		ip_send(frame_rec, icmp_pckt_len);
		res = 1;
	}
	return res;
	
}

void net_poll(void)
{
	char s1[STR_LEN];
	uint16_t len;
	
	while((len = enc28_packet_receive(&frame)) > 0)
	{
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
					
					if(arp_read(&frame, frame.data_len))
						arp_send(&frame, frame.data_len);
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
	
}




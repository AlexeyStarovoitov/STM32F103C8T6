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
extern uint8_t ip_serv[4];
extern uart_ip ip_serv_str;
extern arp_record arp_table[ARP_TABLE_LEN];
extern uint8_t cur_index;
extern uint32_t clk_s;

void USART1_IRQHandler(void)
{
	char s1[STR_LEN];
	uint8_t ans;
	
	ans = USART1->DR;
	USART1->SR &= ~USART_SR_RXNE;
	
	if (ip_serv_str.buf_len > sizeof(ip_serv_str.ip_buf))
		ip_serv_str.buf_len = 0;
	else if(ans == 'a')
	{
		ip_serv_str.ip_buf[ip_serv_str.buf_len] = 0;
		uart_transmit((uint8_t *)"\r\n", 2);
		uart_transmit((uint8_t *)ip_serv_str.ip_buf, ip_serv_str.buf_len);
		uart_transmit((uint8_t *)"\r\n", 2);
		
		ip_str2num((char *)ip_serv_str.ip_buf, ip_serv);
		sprintf(s1,"%03d.%03d.%03d.%03d\r\n", ip_serv[0], ip_serv[1], ip_serv[2], ip_serv[3]);
		uart_transmit((uint8_t *)s1, strlen(s1));
		ip_serv_str.buf_len = 0;
		
		arp_request(ip_serv);
	}
	else
	{
		ip_serv_str.ip_buf[ip_serv_str.buf_len] = ans;
		ip_serv_str.buf_len++;
	}
	
	
}


void EXTI0_IRQHandler(void)
{
	char s1[STR_LEN];
	uint16_t len;
	uint8_t res = 0;
	
	EXTI->PR |= (uint32_t)EXTI_PR_PR0;
	enc28_write_op(BFC, EIE, EIE_INTIE);
	
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
	
	enc28_write_op(BFS, EIE, EIE_INTIE);
}

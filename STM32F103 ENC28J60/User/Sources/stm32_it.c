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

void USART1_IRQHandler(void)
{
	
}


void EXTI0_IRQHandler(void)
{
	char s1[STR_LEN];
	uint16_t len;
	
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
	
	enc28_write_op(BFS, EIE, EIE_INTIE);
}

#include "stm32f10x.h"
#include "tims.h"
#include "gpio.h"
#include "can.h"

void USB_HP_CAN1_TX_IRQHandler(void)
{
	CAN1->TSR |= (CAN_TSR_TXOK0 | CAN_TSR_RQCP0);
}

void USB_LP_CAN1_RX0_IRQHandler(void)
{
	uint8_t rindex = (CAN1->sFIFOMailBox[0].RDTR & CAN_RDT0R_FMI) >> 8;
	uint16_t rx_id = (CAN1->sFIFOMailBox[0].RIR & CAN_RI0R_STID) >> 21;
	uint8_t led_rec = (uint8_t)CAN1->sFIFOMailBox[0].RDLR;
	
	LED_toggle(led_rec);
	
	CAN1->sTxMailBox[0].TIR &= ~CAN_TI0R_STID;
	if((rindex == 0) && (rx_id == RX_F0_ID0))
		CAN1->sTxMailBox[0].TIR |= (TX_ID1 << 21);
	else if((rindex == 1) && (rx_id == RX_F0_ID1))
		CAN1->sTxMailBox[0].TIR |= (TX_ID3 << 21);
		
	led_rec++; 
	
	CAN1->sTxMailBox[0].TIR &= ~CAN_TI0R_RTR;
	CAN1->sTxMailBox[0].TIR &= ~CAN_TI0R_IDE;
	
	CAN1->sTxMailBox[0].TDTR = 1;
	CAN1->sTxMailBox[0].TDLR = led_rec;
	
	CAN1->RF0R |= CAN_RF0R_RFOM0;
	while((CAN1->RF0R) & CAN_RF0R_FMP0)
		;
	CAN1->sTxMailBox[0].TIR |= CAN_TI0R_TXRQ;
	
}

void CAN1_RX1_IRQHandler(void)
{
	uint8_t rindex = (CAN1->sFIFOMailBox[1].RDTR & CAN_RDT1R_FMI) >> 8;
	uint16_t rx_id = (CAN1->sFIFOMailBox[1].RIR & CAN_RI1R_STID) >> 21;
	uint8_t led_rec = (uint8_t)CAN1->sFIFOMailBox[1].RDLR;
	
	LED_toggle(led_rec);
	
	CAN1->sTxMailBox[0].TIR &= ~CAN_TI1R_STID;
	if((rindex == 0) && (rx_id == RX_F1_ID0))
	{
		led_rec++;
		CAN1->sTxMailBox[0].TIR |= (TX_ID2 << 21);
	}
	else if((rindex == 1) && (rx_id == RX_F1_ID1))
	{
		led_rec = 4;
		CAN1->sTxMailBox[0].TIR |= (TX_ID0 << 21);
	}
	
	CAN1->sTxMailBox[0].TIR &= ~CAN_TI0R_RTR;
	CAN1->sTxMailBox[0].TIR &= ~CAN_TI0R_IDE;
	
	CAN1->sTxMailBox[0].TDTR = 1;
	CAN1->sTxMailBox[0].TDLR = led_rec;
	
	CAN1->RF1R |= CAN_RF1R_RFOM1;
	while((CAN1->RF1R) & CAN_RF1R_FMP1)
		;
	
	CAN1->sTxMailBox[0].TIR |= CAN_TI0R_TXRQ;
	
}

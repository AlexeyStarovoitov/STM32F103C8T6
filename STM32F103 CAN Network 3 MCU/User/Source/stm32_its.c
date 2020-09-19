#include "stm32f10x.h"
#include "tims.h"
#include "gpio.h"
#include "can.h"

uint8_t led_id1, led_id2;

void USB_HP_CAN1_TX_IRQHandler(void)
{
	if((CAN1->TSR) & CAN_TSR_RQCP0)
		CAN1->TSR |= (CAN_TSR_TXOK0 | CAN_TSR_RQCP0);
	else if ((CAN1->TSR) & CAN_TSR_RQCP1)
		CAN1->TSR |= (CAN_TSR_TXOK1 | CAN_TSR_RQCP1);
}

void USB_LP_CAN1_RX0_IRQHandler(void)
{
	uint16_t rx_id = (CAN1->sFIFOMailBox[0].RIR & CAN_RI0R_STID) >> 21;
	uint8_t led_rec = (uint8_t)CAN1->sFIFOMailBox[0].RDLR;
		
	if((led_rec == led_id1) && (rx_id == RX_F0_ID))
	{
		LED_toggle(GPIO4);
		CAN1->sTxMailBox[0].TIR &= ~CAN_TI0R_STID;
		CAN1->sTxMailBox[0].TIR |= (TX_F0_ID << 21);
		CAN1->sTxMailBox[0].TIR &= ~CAN_TI0R_RTR;
		CAN1->sTxMailBox[0].TIR &= ~CAN_TI0R_IDE;
		led_id1++;
		if(led_id1 > 7)
			led_id1 = 4;
		CAN1->sTxMailBox[0].TDTR = 1;
		CAN1->sTxMailBox[0].TDLR = led_id1;
		CAN1->sTxMailBox[0].TIR |= CAN_TI0R_TXRQ;
		
//		while (!(CAN1->TSR & CAN_TSR_TXOK0) && !(CAN1->TSR & CAN_TSR_RQCP0))
//		;
//		CAN1->TSR |= (CAN_TSR_TXOK0 | CAN_TSR_RQCP0);
	}
	CAN1->RF0R |= CAN_RF0R_RFOM0;
	while((CAN1->RF0R) & CAN_RF0R_FMP0)
		;
}

void CAN1_RX1_IRQHandler(void)
{
	uint16_t rx_id = (CAN1->sFIFOMailBox[1].RIR & CAN_RI1R_STID) >> 21;
	uint8_t led_rec = (uint8_t)CAN1->sFIFOMailBox[1].RDLR;
		
	if((led_rec == led_id2) && (rx_id == RX_F1_ID))
	{
		LED_toggle(GPIO5);
		CAN1->sTxMailBox[1].TIR &= ~CAN_TI1R_STID;
		CAN1->sTxMailBox[1].TIR |= (TX_F1_ID << 21);
		CAN1->sTxMailBox[1].TIR &= ~CAN_TI1R_RTR;
		CAN1->sTxMailBox[1].TIR &= ~CAN_TI1R_IDE;
		led_id2++;
		if(led_id2 > 4)
			led_id2 = 1;
		CAN1->sTxMailBox[1].TDTR = 1;
		CAN1->sTxMailBox[1].TDLR = led_id2;
		CAN1->sTxMailBox[1].TIR |= CAN_TI1R_TXRQ;
//		while (!(CAN1->TSR & CAN_TSR_TXOK1) && !(CAN1->TSR & CAN_TSR_RQCP1))
//		;
//		CAN1->TSR |= (CAN_TSR_TXOK1 | CAN_TSR_RQCP1);
	}
	
	CAN1->RF1R |= CAN_RF1R_RFOM1;
	while((CAN1->RF1R) & CAN_RF1R_FMP1)
		;
}

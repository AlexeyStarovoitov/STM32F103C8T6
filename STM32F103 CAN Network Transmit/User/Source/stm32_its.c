#include "stm32f10x.h"
#include "tims.h"
#include "gpio.h"
#include "can.h"

uint8_t led_num = 4;

void TIM2_IRQHandler(void)
{
 
	
	TIM2->SR &= ~(TIM_SR_UIF);
	
	CAN1->sTxMailBox[0].TIR &= ~CAN_TI0R_STID ;
	CAN1->sTxMailBox[0].TIR |= (TX_ID1 << 21);
	CAN1->sTxMailBox[0].TIR &= ~CAN_TI0R_RTR;
	CAN1->sTxMailBox[0].TIR &= ~CAN_TI0R_IDE;
	
	led_num = 4;
	CAN1->sTxMailBox[0].TDTR = 1;
	CAN1->sTxMailBox[0].TDLR = led_num;
	
//	CAN1->sTxMailBox[1].TIR &= ~CAN_TI1R_STID ;
//	CAN1->sTxMailBox[1].TIR |= (TX_ID2 << 21);
//	CAN1->sTxMailBox[1].TIR &= ~CAN_TI1R_RTR;
//	CAN1->sTxMailBox[1].TIR &= ~CAN_TI1R_IDE;
//	
//	led_num = 5;
//	CAN1->sTxMailBox[1].TDTR = 1;
//	CAN1->sTxMailBox[1].TDLR = led_num;
//	
//	CAN1->sTxMailBox[2].TIR &= ~CAN_TI2R_STID ;
//	CAN1->sTxMailBox[2].TIR |= (TX_ID3 << 21);
//	CAN1->sTxMailBox[2].TIR &= ~CAN_TI2R_RTR;
//	CAN1->sTxMailBox[2].TIR &= ~CAN_TI2R_IDE;
//	
//	led_num = 6;
//	CAN1->sTxMailBox[2].TDTR = 1;
//	CAN1->sTxMailBox[2].TDLR = led_num;
//		
	CAN1->sTxMailBox[0].TIR |= CAN_TI0R_TXRQ;
//	CAN1->sTxMailBox[1].TIR |= CAN_TI1R_TXRQ;
//	CAN1->sTxMailBox[2].TIR |= CAN_TI2R_TXRQ;
	

}

void USB_HP_CAN1_TX_IRQHandler(void)
{
	switch((CAN1->TSR) & CAN_TX_CMP_MASK)
	{
		case CAN_TSR_RQCP0:
			CAN1->TSR |= (CAN_TSR_TXOK0 | CAN_TSR_RQCP0);
		
			CAN1->sTxMailBox[1].TIR &= ~CAN_TI1R_STID ;
			CAN1->sTxMailBox[1].TIR |= (TX_ID2 << 21);
			CAN1->sTxMailBox[1].TIR &= ~CAN_TI1R_RTR;
			CAN1->sTxMailBox[1].TIR &= ~CAN_TI1R_IDE;
	
			led_num = 5;
			CAN1->sTxMailBox[1].TDTR = 1;
			CAN1->sTxMailBox[1].TDLR = led_num;
			CAN1->sTxMailBox[1].TIR |= CAN_TI1R_TXRQ;
		break;
		case CAN_TSR_RQCP1:
			CAN1->TSR |= (CAN_TSR_TXOK1 | CAN_TSR_RQCP1);
		
			CAN1->sTxMailBox[2].TIR &= ~CAN_TI2R_STID ;
			CAN1->sTxMailBox[2].TIR |= (TX_ID3 << 21);
			CAN1->sTxMailBox[2].TIR &= ~CAN_TI2R_RTR;
			CAN1->sTxMailBox[2].TIR &= ~CAN_TI2R_IDE;
	
			led_num = 6;
			CAN1->sTxMailBox[2].TDTR = 1;
			CAN1->sTxMailBox[2].TDLR = led_num;
			CAN1->sTxMailBox[2].TIR |= CAN_TI2R_TXRQ;
		break;
		case CAN_TSR_RQCP2:
			CAN1->TSR |= (CAN_TSR_TXOK2 | CAN_TSR_RQCP2);
			led_num = 4;
		break;
		
	}
}


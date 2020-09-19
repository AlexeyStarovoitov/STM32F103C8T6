#include "stm32f10x.h"
#include "can.h"
#include "gpio.h"

extern uint8_t led_id1, led_id2;
static void can_gpio_init(void);

void can_init(void)
{
	can_gpio_init();
	
	RCC->APB1ENR |=  RCC_APB1ENR_CAN1EN;
	
	CAN1->MCR |= CAN_MCR_INRQ;
	while (!((CAN1->MSR) & CAN_MSR_INAK))
		;
	
	CAN1->MCR &= ~CAN_MCR_SLEEP;
	while (((CAN1->MSR) & CAN_MSR_SLAK))
		;
	
	
	// Baudrate = 500 KBits/s
	CAN1->BTR |= CAN1_BRP;
	CAN1->BTR &= ~CAN_BTR_TS1;
	CAN1->BTR &= ~CAN_BTR_TS2;
	CAN1->BTR &= ~CAN_BTR_SJW;
	
	CAN1->BTR |= CAN1_TS1;
	CAN1->BTR |= CAN1_TS2;
	CAN1->BTR |= CAN1_SJW;
	
	
	CAN1->MCR &= ~CAN_MCR_DBF;
	CAN1->MCR &= ~CAN_MCR_TTCM;
	CAN1->MCR &= ~CAN_MCR_ABOM;
	CAN1->MCR &= ~CAN_MCR_AWUM;
	CAN1->MCR &= ~CAN_MCR_NART;
	CAN1->MCR &= ~CAN_MCR_RFLM;
	CAN1->MCR |= CAN_MCR_TXFP;
	
	CAN1->IER |= (CAN_IER_TMEIE | CAN_IER_FMPIE0 | CAN_IER_FMPIE1);
	
	CAN1->FMR |= CAN_FMR_FINIT;
	
	// FIFO 0 filters
	CAN1->FA1R &= ~CAN_FA1R_FACT0; 
	CAN1->FM1R |= CAN_FM1R_FBM0;// identifier list mode
	CAN1->FS1R &= ~CAN_FS1R_FSC0; // 16-bit mode
	CAN1->FFA1R &= ~CAN_FFA1R_FFA0;
	CAN1->sFilterRegister[0].FR1 = (uint32_t)(RX_F0_ID << 5); 
	CAN1->FA1R |= CAN_FA1R_FACT0;
	
	// FIFO 1 filters
	CAN1->FA1R &= ~CAN_FA1R_FACT1; 
	CAN1->FM1R |= CAN_FM1R_FBM1;// identifier list mode
	CAN1->FS1R &= ~CAN_FS1R_FSC1; // 16-bit mode
	CAN1->FFA1R |= CAN_FFA1R_FFA1;
	CAN1->sFilterRegister[1].FR1 = (uint32_t)(RX_F1_ID << 5); 
	CAN1->FA1R |= CAN_FA1R_FACT1;
	
	CAN1->FMR &= ~CAN_FMR_FINIT;
	
	CAN1->MCR &= ~CAN_MCR_INRQ;
	while (((CAN1->MSR) & CAN_MSR_INAK))
		;
	
	NVIC_SetPriority(USB_HP_CAN1_TX_IRQn, CAN1_TX_PRIOR);
	NVIC_EnableIRQ(USB_HP_CAN1_TX_IRQn);
	
	NVIC_SetPriority(USB_LP_CAN1_RX0_IRQn, CAN1_RX0_PRIOR);
	NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);
	
	NVIC_SetPriority(CAN1_RX1_IRQn, CAN1_RX1_PRIOR);
	NVIC_EnableIRQ(CAN1_RX1_IRQn);
}

static void can_gpio_init(void)
{
	RCC->APB2ENR |= (RCC_APB2ENR_IOPAEN | RCC_APB2ENR_AFIOEN);
	
	GPIOA->CRH &= ~CAN_MODE_MASK;
	GPIOA->CRH |= ((GPIO_IN << GPIO11_MODE*4) | (GPIO_IN_PU_PD << (GPIO11_MODE*4)) | (GPIO_OUT_50_MHz << GPIO12_MODE*4) | (GPIO_AF_PP << (GPIO12_MODE*4)));
	
}

void can_start(void)
{
	led_id1 = 4;
	led_id2 = 1;
	
	CAN1->sTxMailBox[0].TIR &= ~CAN_TI0R_STID ;
	CAN1->sTxMailBox[0].TIR &= ~CAN_TI0R_RTR;
	CAN1->sTxMailBox[0].TIR &= ~CAN_TI0R_IDE;
	CAN1->sTxMailBox[0].TIR |= (TX_F0_ID << 21);
	
	CAN1->sTxMailBox[0].TDTR = 1;
	CAN1->sTxMailBox[0].TDLR = led_id1;
	
	
	CAN1->sTxMailBox[1].TIR &= ~CAN_TI1R_STID ;
	CAN1->sTxMailBox[1].TIR &= ~CAN_TI1R_RTR;
	CAN1->sTxMailBox[1].TIR &= ~CAN_TI1R_IDE;
	CAN1->sTxMailBox[1].TIR |= (TX_F1_ID << 21);
	
	CAN1->sTxMailBox[1].TDTR = 1;
	CAN1->sTxMailBox[1].TDLR = led_id2;
	
	CAN1->sTxMailBox[0].TIR |= CAN_TI0R_TXRQ;
	CAN1->sTxMailBox[1].TIR |= CAN_TI1R_TXRQ;
	
//	while (!(CAN1->TSR & CAN_TSR_TXOK0) && !(CAN1->TSR & CAN_TSR_RQCP0))
//		;
//	CAN1->TSR |= (CAN_TSR_TXOK0 | CAN_TSR_RQCP0);
//	
//	while (!(CAN1->TSR & CAN_TSR_TXOK1) && !(CAN1->TSR & CAN_TSR_RQCP1))
//		;
//	CAN1->TSR |= (CAN_TSR_TXOK1 | CAN_TSR_RQCP1);
}



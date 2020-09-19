#include "stm32f10x.h"
#include "can.h"
#include "gpio.h"

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
	CAN1->MCR &= ~CAN_MCR_TXFP;
	
	
	CAN1->FMR |= CAN_FMR_FINIT;
	CAN1->FA1R &= ~CAN_FA1R_FACT0; 
	
	CAN1->FM1R |= CAN_FM1R_FBM0;// identifier list mode
	CAN1->FS1R &= ~CAN_FS1R_FSC0; // 16-bit mode
	CAN1->FFA1R &= ~CAN_FFA1R_FFA0;
	CAN1->sFilterRegister[0].FR1 = (uint32_t)(RX_ID << 5); 
	
	CAN1->FA1R |= CAN_FA1R_FACT0;
	CAN1->FMR &= ~CAN_FMR_FINIT;
	
	CAN1->MCR &= ~CAN_MCR_INRQ;
	while (((CAN1->MSR) & CAN_MSR_INAK))
		;
	
}

static void can_gpio_init(void)
{
	RCC->APB2ENR |= (RCC_APB2ENR_IOPAEN | RCC_APB2ENR_AFIOEN);
	
	GPIOA->CRH &= ~CAN_MODE_MASK;
	GPIOA->CRH |= ((GPIO_IN << GPIO11_MODE*4) | (GPIO_IN_PU_PD << (GPIO11_MODE*4)) | (GPIO_OUT_50_MHz << GPIO12_MODE*4) | (GPIO_AF_PP << (GPIO12_MODE*4)));
	
}




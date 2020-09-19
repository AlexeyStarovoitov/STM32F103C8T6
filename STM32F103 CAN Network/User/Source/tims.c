#include "stm32f10x.h"
#include "tims.h"
#include "gpio.h"
#include "can.h"

void tim2_init(void)
{
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	
	TIM2->CR1 |= TIM_CR1_ARPE;
	TIM2->DIER |= TIM_DIER_UIE;
	TIM2->PSC = TIM2_PSC;
	TIM2->ARR = TIM2_ARR;
	
	NVIC_SetPriority(TIM2_IRQn , TIM2_PRIOR);
	NVIC_EnableIRQ(TIM2_IRQn);
	
}

void tim3_init(void)
{
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
	
	TIM3->CR1 |= TIM_CR1_ARPE;
	TIM3->PSC = TIM3_PSC;
}

void tim2_start(void)
{
	TIM2->CR1 |= TIM_CR1_CEN;
}

void tim3_start(void)
{
	TIM3->CR1 |= TIM_CR1_CEN;
}

void tim3_stop(void)
{
	TIM3->CR1 &= ~TIM_CR1_CEN;
}

void delay_us(uint32_t t_us)
{
	tim3_init();
	
	TIM3->ARR = t_us-1;
	TIM3->EGR |= TIM_EGR_UG;
	
	while(!((TIM3->SR) & TIM_SR_UIF))
		;
	TIM3->SR &= ~(TIM_SR_UIF);
	
	tim3_start();
	while(!((TIM3->SR) & TIM_SR_UIF))
		;
	tim3_stop();
	TIM3->SR &= ~(TIM_SR_UIF);
	
}

void TIM2_IRQHandler(void)
{
	static uint8_t led_num = 4;
	static uint8_t led_rec;
	
	TIM2->SR &= ~(TIM_SR_UIF);
	
	
//	LED_toggle(LED_GPIO_13);
	
	CAN1->sTxMailBox[0].TIR &= ~CAN_TI0R_STID ;
	CAN1->sTxMailBox[0].TIR |= (TX_ID << 21);
	CAN1->sTxMailBox[0].TIR &= ~CAN_TI0R_RTR;
	CAN1->sTxMailBox[0].TIR &= ~CAN_TI0R_IDE;
	
	CAN1->sTxMailBox[0].TDTR = 1;
	CAN1->sTxMailBox[0].TDLR = led_num;
		
	CAN1->sTxMailBox[0].TIR |= CAN_TI0R_TXRQ;
	while (!(CAN1->TSR & CAN_TSR_TXOK0) && !(CAN1->TSR & CAN_TSR_RQCP0))
		;
	CAN1->TSR |= (CAN_TSR_TXOK0 | CAN_TSR_RQCP0);
		
	led_num++;
	if(led_num > 7)
		led_num = 4;
		
	while(!((CAN1->RF0R) & CAN_RF0R_FMP0))
		;
		
	led_rec = (uint8_t)CAN1->sFIFOMailBox[0].RDLR;
		
	CAN1->RF0R |= CAN_RF0R_RFOM0;
	while((CAN1->RF0R) & CAN_RF0R_FMP0)
		;
}

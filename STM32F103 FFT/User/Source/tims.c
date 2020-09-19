#include "stm32f10x.h"
#include "tims.h"
#include "gpio.h"


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
	TIM2->SR &= ~(TIM_SR_UIF);
//	LED_toggle(LED_GPIO_13);
}

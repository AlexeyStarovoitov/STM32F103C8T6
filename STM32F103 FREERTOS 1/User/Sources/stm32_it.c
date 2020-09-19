#include "stm32f10x.h"
#include "gpio.h"
#include "spi.h"
#include "uart.h"


void USART1_IRQHandler(void)
{
	
}


void EXTI0_IRQHandler(void)
{
	
}

void TIM2_IRQHandler(void)
{
	TIM2->SR &= ~(TIM_SR_UIF);
	LED_toggle(GPIO13);
}

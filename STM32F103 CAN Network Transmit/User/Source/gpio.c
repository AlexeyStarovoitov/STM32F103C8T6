#include "stm32f10x.h"
#include "gpio.h"
#include "can.h"


/*
	LED1 - GPIOD12, LED2 - GPIOD13, LED3 - GPIOD14, LED4 - GPIOD15
*/

void LED_GPIO_Init(void)
{
	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN ;
	
	GPIOC->CRH |= ((GPIO_OUT_50_MHz << GPIO13_MODE*4) | (GPIO_OUT_PP << GPIO13_MODE*4));
	LED_Off(LED_GPIO_13);
	
}

void LED_On(uint8_t led_gpio)
{
	GPIOC->BSRR |= (GPIO_S << led_gpio);
}
	
void LED_Off(uint8_t led_gpio)
{
	GPIOC->BRR |= (GPIO_R << led_gpio);
}

void LED_toggle(uint8_t led_gpio)
{
	if (Led_State(led_gpio))
		LED_Off(led_gpio);
	else
		LED_On(led_gpio);
}


uint32_t Led_State(uint8_t led_gpio)
{
	return (GPIOC->ODR & (1 << led_gpio));
}

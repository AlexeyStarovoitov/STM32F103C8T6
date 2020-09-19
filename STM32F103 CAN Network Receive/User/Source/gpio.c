#include "stm32f10x.h"
#include "gpio.h"
#include "can.h"

void LED_CAN_GPIO_Init(void)
{
	GPIOA->CRL &= ~CAN_LED_GPIO_MASK;
	GPIOA->CRL |= ((GPIO_OUT_50_MHz << GPIO4_MODE*4) | ( GPIO_OUT_PP << (GPIO4_MODE*4))  | (GPIO_OUT_50_MHz << GPIO5_MODE*4) | ( GPIO_OUT_PP << (GPIO5_MODE*4)) | (GPIO_OUT_50_MHz << GPIO6_MODE*4) | ( GPIO_OUT_PP << (GPIO6_MODE*4)) | (GPIO_OUT_50_MHz << GPIO7_MODE*4) | ( GPIO_OUT_PP << (GPIO7_MODE*4)));
	
	LED_Off(GPIO4);
	LED_Off(GPIO5);
	LED_Off(GPIO6);
	LED_Off(GPIO7);
	
//	LED_On(GPIO4);
//	LED_On(GPIO5);
//	LED_On(GPIO6);
//	LED_On(GPIO7);
}

void LED_On(uint8_t led_gpio)
{
	GPIOA->BSRR |= (GPIO_S << led_gpio);
}
	
void LED_Off(uint8_t led_gpio)
{
	GPIOA->BRR |= (GPIO_R << led_gpio);
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
	return (GPIOA->ODR & (1 << led_gpio));
}

#include "stm32f10x.h"
#include "gpio.h"

//void exti_gpio_init(void)
//{
//	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
//	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
//	
//	GPIOA->CRL &= ~EXTI_GPIO_MASK;
//	GPIOA->CRL |= ((GPIO_IN << GPIO0_MODE*4) | ( GPIO_IN_PU_PD << (GPIO0_MODE*4)));
//	AFIO->EXTICR[0] &= ~AFIO_EXTICR1_EXTI0;
//	AFIO->EXTICR[0] |= AFIO_EXTICR1_EXTI0_PA;
//	
//	EXTI->IMR |= EXTI_IMR_MR0;
//	EXTI->FTSR |= EXTI_FTSR_TR0;
//	
//	NVIC_SetPriority(EXTI0_IRQn, EXTI0_PRIOR);
//	NVIC_EnableIRQ(EXTI0_IRQn);
//	
//	
//}

void LED_GPIO_Init(void)
{
	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN ;
	
	GPIOC->CRH |= ((GPIO_OUT_50_MHz << GPIO13_MODE*4) | (GPIO_OUT_PP << GPIO13_MODE*4));
	LED_Off(GPIO13);
	
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

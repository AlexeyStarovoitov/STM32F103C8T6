#include "stm32f10x.h"
#include "gpio.h"
#include "uart.h"

static void gpio_uart_init(void)
{
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
	
	GPIOA->CRH &= ~UART_GPIO_MASK;
	GPIOA->CRH |= ((GPIO_OUT_50_MHz << GPIO9_MODE*4) | ( GPIO_AF_PP << (GPIO9_MODE*4))  | (GPIO_IN << GPIO10_MODE*4) | ( GPIO_IN_FLT << (GPIO10_MODE*4)));
	
}

void uart_init(void)
{
	gpio_uart_init();
	
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN ;
	USART1->CR1 |= USART_CR1_UE;
	USART1->CR1 |= USART_CR1_RXNEIE;
	USART1->CR1 &= ~USART_CR1_M;
	USART1->CR2 &= ~USART_CR2_STOP;
	USART1->BRR |= UART_BAUD_SYS_40_230400;
	USART1->CR1 |= (USART_CR1_TE | USART_CR1_RE);
	
	NVIC_SetPriority(USART1_IRQn, USART_PRIOR);
	NVIC_EnableIRQ(USART1_IRQn);
	
}

void uart_transmit(uint8_t *buf, uint16_t len)
{
	uint16_t i;
	
	for(i = 0; i < len; i++)
	{
		USART1->DR = buf[i];
		while(!(USART1->SR & USART_SR_TXE))
			;
	}
	while(!(USART1->SR & USART_SR_TC))
			;
	USART1->SR &= ~USART_SR_TC;
}

void uart_receive(uint8_t *dat)
{
	*dat = USART1->DR;
	USART1->SR &= ~USART_SR_RXNE;
}

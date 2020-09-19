#include "stm32f10x.h"
#include "gpio.h"
#include "spi.h"

void nss_desel(void)
{
	GPIOA->BSRR |= (GPIO_S << GPIO4);
}

void nss_sel(void)
{
	GPIOA->BRR |= (GPIO_R << GPIO4);
}

static void spi_gpio_init(void)
{
	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
	
	GPIOA->CRL &= ~SPI_GPIO_MASK;
	GPIOA->CRL |= ((GPIO_OUT_50_MHz << GPIO4_MODE*4) | ( GPIO_OUT_PP << (GPIO4_MODE*4))  | (GPIO_OUT_50_MHz << GPIO5_MODE*4) | ( GPIO_AF_PP << (GPIO5_MODE*4)) | (GPIO_OUT_50_MHz << GPIO6_MODE*4) | ( GPIO_AF_PP << (GPIO6_MODE*4)) | (GPIO_OUT_50_MHz << GPIO7_MODE*4) | ( GPIO_AF_PP << (GPIO7_MODE*4)));
	
	nss_desel();
}

void spi_master_init(void)
{
	spi_gpio_init();
	
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
	
	SPI1->CR1 &= ~(SPI_CR1_BR); 
	SPI1->CR1 |= SPI_BAUD_10_SYS_40;
	SPI1->CR1 &= ~(SPI_CR1_CPHA | SPI_CR1_CPOL | SPI_CR1_RXONLY | SPI_CR1_BIDIMODE);
	SPI1->CR1 &= ~SPI_CR1_DFF;
	SPI1->CR1 &= ~SPI_CR1_LSBFIRST;
	
	SPI1->CR1 |= (SPI_CR1_SSM | SPI_CR1_SSI);
	
	SPI1->CR1 |= SPI_CR1_MSTR;
	SPI1->CR1 &=  ~SPI_CR1_SPE;
}

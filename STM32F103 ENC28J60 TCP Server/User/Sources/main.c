#include "stm32f10x.h"
#include "gpio.h"
#include "spi.h"
#include "enc28j60.h"
#include "uart.h"
#include "net.h"
#include "tims.h"



int main(void)
{
	
	SystemInit();

	spi_master_init();
	exti_gpio_init();
	uart_init();
	enc28_init();
	net_init();
	tim2_init();
	tim2_start();
	
	while(1)
	{
	}
}

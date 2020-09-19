#include "stm32f10x.h"
#include "tims.h"
#include "gpio.h"
#include "can.h"


int main(void)
{
	SystemInit();
	//tim2_init();
	can_init();
	LED_CAN_GPIO_Init();
	can_start();
	//tim2_start();

	while(1)
	{

	}
}

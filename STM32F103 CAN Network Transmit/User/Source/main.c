#include "stm32f10x.h"
#include "tims.h"
#include "gpio.h"
#include "can.h"


int main(void)
{
	SystemInit();
	tim2_init();
	can_init();
	tim2_start();
	
	while(1)
	{

	}
}

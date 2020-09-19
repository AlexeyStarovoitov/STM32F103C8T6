#include "stm32f10x.h"
#include "FreeRTOS.h"
#include "free_rtos_tasks.h"
#include "task.h"
#include "queue.h"
#include "gpio.h"
#include "tims.h"

int8_t i;

int main(void)
{
	
	SystemInit();
	LED_GPIO_Init();
	tim2_init();
	tim2_start();
	i = xTaskCreate(vTask_LED, "LED13", 64, NULL, 10, NULL);
	vTaskStartScheduler();

	while(1)
	{

	}
}

#include "stm32f10x.h"
#include "FreeRTOS.h"
#include "free_rtos_tasks.h"
#include "task.h"
#include "queue.h"
#include "gpio.h"


void vTask_LED(void *p_param)
{
	while(1)
	{
		LED_toggle(GPIO13);
		vTaskDelay(1000);
		LED_toggle(GPIO13);
		vTaskDelay(1000);
	}
}

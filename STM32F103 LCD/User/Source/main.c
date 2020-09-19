#include "stm32f10x.h"
#include "tims.h"
#include "gpio.h"
#include "lcd.h"

int main(void)
{
	SystemInit();
	
	lcd_4bit_init();
	lcd_wr_str("Hi from stm32!", 0x0, 0);
	lcd_wr_str("StarCorp", 0x40, 0);
//	lcd_wr_data(REG_TD);
	while(1)
	{
	}
}

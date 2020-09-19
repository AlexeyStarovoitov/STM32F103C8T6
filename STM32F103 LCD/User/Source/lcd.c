#include "stm32f10x.h"
#include "tims.h"
#include "gpio.h"
#include "lcd.h"


static void lcd_gpio_reset(void)
{
	GPIOA->BRR |= ((GPIO_R << GPIO1) | (GPIO_R << GPIO2) | (GPIO_R << GPIO3) | (GPIO_R << GPIO4) | (GPIO_R << GPIO5) | (GPIO_R << GPIO6) | (GPIO_R << GPIO7));
}

static void lcd_e_set(void)
{
	GPIOA->BSRR |= (GPIO_S << GPIO7);
}


static void lcd_e_reset(void)
{
	GPIOA->BRR |= (GPIO_R << GPIO7);
}

static void lcd_a0_set(void)
{
	GPIOA->BSRR |= (GPIO_S << GPIO6);
}


static void lcd_a0_reset(void)
{
	GPIOA->BRR |= (GPIO_R << GPIO6);
}



static void lcd_rw_reset(void)
{
	GPIOA->BRR |= (GPIO_R << GPIO5);
}

static void lcd_d4_set(void)
{
	GPIOA->BSRR |= (GPIO_S << GPIO4);
}


static void lcd_d4_reset(void)
{
	GPIOA->BRR |= (GPIO_R << GPIO4);
}


static void lcd_d3_set(void)
{
	GPIOA->BSRR |= (GPIO_S << GPIO3);
}


static void lcd_d3_reset(void)
{
	GPIOA->BRR |= (GPIO_R << GPIO3);
}

static void lcd_d2_set(void)
{
	GPIOA->BSRR |= (GPIO_S << GPIO2);
}


static void lcd_d2_reset(void)
{
	GPIOA->BRR |= (GPIO_R << GPIO2);
}


static void lcd_d1_set(void)
{
	GPIOA->BSRR |= (GPIO_S << GPIO1);
}


static void lcd_d1_reset(void)
{
	GPIOA->BRR |= (GPIO_R << GPIO1);
}

static void lcd_4bit_wr_dat(uint8_t dat)
{
	if((dat >> 3) & 0x01) {lcd_d4_set();} else {lcd_d4_reset();}
	if((dat >> 2) & 0x01) {lcd_d3_set();} else {lcd_d3_reset();}
	if((dat >> 1) & 0x01) {lcd_d2_set();} else {lcd_d2_reset();}
	if(dat & 0x01) {lcd_d1_set();} else {lcd_d1_reset();}
}
	
/*
PA1-PA4: DB4-DB7; PA5-PA7: R/W, A0, E;
*/
static void lcd_gpio_init(void)
{
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
	
	GPIOA->CRL &= ~LCD_GPIO_MASK;
	GPIOA->CRL |= ((GPIO_OUT_50_MHz << GPIO1_MODE*4) | (GPIO_OUT_PP << (GPIO1_MODE*4)) | (GPIO_OUT_50_MHz << GPIO2_MODE*4) | ( GPIO_OUT_PP << (GPIO2_MODE*4)) | (GPIO_OUT_50_MHz << GPIO3_MODE*4) | ( GPIO_OUT_PP << (GPIO3_MODE*4))\
	 | (GPIO_OUT_50_MHz << GPIO4_MODE*4) | ( GPIO_OUT_PP << (GPIO4_MODE*4))  | (GPIO_OUT_50_MHz << GPIO5_MODE*4) | ( GPIO_OUT_PP << (GPIO5_MODE*4)) | (GPIO_OUT_50_MHz << GPIO6_MODE*4) | ( GPIO_OUT_PP << (GPIO6_MODE*4)) | (GPIO_OUT_50_MHz << GPIO7_MODE*4) | ( GPIO_OUT_PP << (GPIO7_MODE*4)));
	lcd_gpio_reset();
}



void lcd_4bit_init(void)
{
	
	lcd_gpio_init();
	delay_us(30000);
	
	for(uint8_t i = 0; i < 3; i++)
	{
		lcd_a0_reset();
		lcd_rw_reset();
		lcd_e_reset();
	
		lcd_e_set();
		lcd_4bit_wr_dat(0x03);
		delay_us(2);
		lcd_e_reset();
		
		delay_us(60);
	}	
	
	
	lcd_a0_reset();
	lcd_rw_reset();
	lcd_e_reset();
	
	lcd_e_set();
	lcd_4bit_wr_dat(0x02);
	delay_us(2);
	lcd_e_reset();
	
	delay_us(60);
	
	lcd_4bit_wr_cmd(DISP_FUNC_SET_4BIT_PG0);
	lcd_4bit_wr_cmd(DISPLAY_OFF_C0_B0);
	lcd_4bit_wr_cmd(DISPLAY_CLR);
	lcd_4bit_wr_cmd(DISP_MODE_SET_ID1_SH0);
	
	lcd_4bit_wr_cmd(DISPLAY_ON_C1_B1);
	lcd_4bit_wr_cmd(DISPLAY_CLR);
	lcd_4bit_wr_cmd(DISPLAY_RET_HM);

}

//lcd_cmd = A0 R/W xxxxxxxx (DB7-DB0)
void lcd_4bit_wr_cmd(uint16_t lcd_cmd)
{
	uint8_t lcd_cmd1, lcd_cmd2;
	
	if(lcd_cmd & A0_MASK)
		lcd_a0_set();
	else
		lcd_a0_reset();
	
	lcd_rw_reset();
	lcd_e_reset();
	
	lcd_cmd1 = (lcd_cmd & DATA_MHB_MASK) >> 4;
	lcd_cmd2 = lcd_cmd & DATA_LHB_MASK;
	
	lcd_e_set();
	lcd_4bit_wr_dat(lcd_cmd1);
	delay_us(2);
	lcd_e_reset();
	delay_us(2);
	
	lcd_e_set();
	lcd_4bit_wr_dat(lcd_cmd2);
	delay_us(2);
	lcd_e_reset();
	
	if (lcd_cmd == DISPLAY_CLR)
		delay_us(3500);
	else
		delay_us(60);
	
	
}

void lcd_wr_data(uint8_t datas)
{
	lcd_4bit_wr_cmd(DISP_WR_DATA | datas);
}

void lcd_set_daddr(uint8_t addr)
{
	lcd_4bit_wr_cmd(DISP_SET_DDRAM_ADDR | addr);
}

void lcd_wr_str(uint8_t *s, uint8_t st_addr, uint8_t clr)
{
	lcd_set_daddr(st_addr);
	if(clr)
		lcd_4bit_wr_cmd(DISPLAY_CLR);
	while(*s)
		lcd_wr_data(*s++);
	
}

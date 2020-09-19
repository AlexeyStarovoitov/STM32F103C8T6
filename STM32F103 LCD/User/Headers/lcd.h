#ifndef LCD_H_
#define LCD_H_

#define DISPLAY_OFF_C0_B0	((uint16_t)0x0008)
#define DISPLAY_OFF_C0_B1	((uint16_t)0x0009)
#define DISPLAY_OFF_C1_B0	((uint16_t)0x000A)
#define DISPLAY_OFF_C1_B1	((uint16_t)0x000B)

#define DISPLAY_ON_C0_B0	((uint16_t)0x000C)
#define DISPLAY_ON_C0_B1	((uint16_t)0x000D)
#define DISPLAY_ON_C1_B0	((uint16_t)0x000E)
#define DISPLAY_ON_C1_B1	((uint16_t)0x000F)

#define DISPLAY_CLR	((uint16_t)0x0001)
#define DISPLAY_RET_HM	((uint16_t)0x0002)

#define DISP_MODE_SET_ID0_SH0	((uint16_t)0x0004)
#define DISP_MODE_SET_ID0_SH1	((uint16_t)0x0005)
#define DISP_MODE_SET_ID1_SH0	((uint16_t)0x0006)
#define DISP_MODE_SET_ID1_SH1	((uint16_t)0x0007)

#define DISP_CUR_SHFT_SC0_RL0	((uint16_t)0x0010)
#define DISP_CUR_SHFT_SC0_RL1	((uint16_t)0x0014)
#define DISP_CUR_SHFT_SC1_RL0	((uint16_t)0x0018)
#define DISP_CUR_SHFT_SC1_RL1	((uint16_t)0x001C)

#define DISP_FUNC_SET_4BIT_PG0	((uint16_t)0x0028)
#define DISP_FUNC_SET_4BIT_PG1	((uint16_t)0x002A)
#define DISP_FUNC_SET_8BIT_PG0	((uint16_t)0x0038)
#define DISP_FUNC_SET_8BIT_PG1	((uint16_t)0x003A)

#define DISP_SET_DGRAM_ADDR	((uint16_t)1<<6)
#define DISP_SET_DDRAM_ADDR	((uint16_t)1<<7)

#define DISP_WR_DATA	((uint16_t)1<<9)

#define A0_RW_MASK	((uint16_t)0x0300)
#define A0_MASK	((uint16_t)0x0200)
#define RW_MASK	((uint16_t)0x0100)
#define LCD_DATA_GPIO_MASK ((uint32_t)0x00001E)
#define LCD_CMD_GPIO_MASK ((uint32_t)0x0000FE)
#define DATA_MHB_MASK ((uint16_t)0x00F0)
#define DATA_LHB_MASK ((uint16_t)0x000F)

#define REG_TD ((uint8_t)0x018)

void lcd_4bit_init(void);
void lcd_4bit_wr_cmd(uint16_t lcd_cmd);
void lcd_wr_data(uint8_t datas);
void lcd_set_daddr(uint8_t addr);
void lcd_wr_str(uint8_t *s, uint8_t st_addr, uint8_t clr);

#endif

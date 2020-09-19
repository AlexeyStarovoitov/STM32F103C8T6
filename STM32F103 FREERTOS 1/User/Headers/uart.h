#ifndef UART_H_
#define UART_H_

#define UART_GPIO_MASK (GPIO_CRH_MODE9  | GPIO_CRH_MODE10 | GPIO_CRH_CNF9 | GPIO_CRH_CNF10)
#define UART_BAUD_SYS_40_230400 ((uint16_t)0xAE)
#define USART_PRIOR ((uint32_t)1)

void uart_init(void);
void uart_transmit(uint8_t *buf, uint16_t len);
void uart_receive(uint8_t *dat);

#endif

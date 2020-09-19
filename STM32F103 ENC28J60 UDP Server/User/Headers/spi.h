#ifndef SPI_H_
#define SPI_H_

#define SPI_GPIO_MASK (GPIO_CRL_MODE4  | GPIO_CRL_MODE5 | GPIO_CRL_MODE6  | GPIO_CRL_MODE7 |  GPIO_CRL_CNF4 | GPIO_CRL_CNF5 | GPIO_CRL_CNF6 | GPIO_CRL_CNF7)
#define SPI_BAUD_10_SYS_40 SPI_CR1_BR_0

void spi_master_init(void);
void nss_desel(void);
void nss_sel(void);

#endif

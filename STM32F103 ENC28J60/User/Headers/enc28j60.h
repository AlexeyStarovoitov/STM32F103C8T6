#ifndef ENC28_H_
#define ENC28_H_

#define ENC28_LED_MASK (GPIO_CRL_MODE4 | GPIO_CRL_CNF4)
#define ENC28_ADDR_MASK ((uint8_t)0x1F)
#define BANK_REG_MASK	((uint8_t)0x60)
#define BANK_0	((uint8_t)0x00)
#define BANK_1	((uint8_t)0x20)
#define BANK_2	((uint8_t)0x40)
#define BANK_3	((uint8_t)0x60)
#define ETH_REG	((uint8_t)0x80)

#define ENC28J60_MAXFRAME ((uint16_t)512)
#define MAX_FRAMELEN	((uint16_t)1500)
#define RX_ST ((uint16_t)0x0000)
#define RX_ND ((uint16_t)0x0BFF)
#define TX_ST ((uint16_t)0x0C00)
#define TX_ND ((uint16_t)0x11FF)

typedef struct
{
	uint8_t dest_addr[6];
	uint8_t src_addr[6];
	uint16_t type;
} enc28_frame_header;

typedef struct
{
	enc28_frame_header hdr;
	uint8_t *data;
	uint16_t data_len;
} enc28_frame;

// Common Used Regs
#define EIE ((uint8_t)0x1B)
#define EIR ((uint8_t)0x1C)
#define ESTAT ((uint8_t)0x1D)
#define ECON2 ((uint8_t)0x1E)
#define ECON1 ((uint8_t)0x1F)

//Bank 0 Regs
#define ERDPT ((uint8_t)(0x00 | BANK_0 | ETH_REG))
#define EWRPT ((uint8_t)(0x02| BANK_0 | ETH_REG))
#define ETXST ((uint8_t)(0x04 | BANK_0 | ETH_REG))
#define ETXND ((uint8_t)(0x06 | BANK_0 | ETH_REG))
#define ERXST ((uint8_t)(0x08 | BANK_0 | ETH_REG))
#define ERXND ((uint8_t)(0x0A | BANK_0 | ETH_REG))
#define ERXRDPT ((uint8_t)(0x0C | BANK_0 | ETH_REG))
#define ERXWRPT ((uint8_t)(0x0E | BANK_0 | ETH_REG))
#define EDMAST ((uint8_t)(0x10 | BANK_0 | ETH_REG))
#define EDMAND ((uint8_t)(0x12| BANK_0 | ETH_REG))
#define EDMADST ((uint8_t)(0x14| BANK_0 | ETH_REG))
#define EDMACS ((uint8_t)(0x16| BANK_0 | ETH_REG))

//Bank 1 Regs

#define EHT0 ((uint8_t)(0x00 | BANK_1 | ETH_REG))
#define EHT1 ((uint8_t)(0x01 | BANK_1 | ETH_REG))
#define EHT2 ((uint8_t)(0x02 | BANK_1 | ETH_REG))
#define EHT3 ((uint8_t)(0x03 | BANK_1 | ETH_REG))
#define EHT4 ((uint8_t)(0x04 | BANK_1 | ETH_REG))
#define EHT5 ((uint8_t)(0x05 | BANK_1 | ETH_REG))
#define EHT6 ((uint8_t)(0x06 | BANK_1 | ETH_REG))
#define EHT7 ((uint8_t)(0x07 | BANK_1 | ETH_REG))
#define EPMM0 ((uint8_t)(0x08 | BANK_1 | ETH_REG))
#define EPMM1 ((uint8_t)(0x09 | BANK_1 | ETH_REG))
#define EPMM2 ((uint8_t)(0x0A | BANK_1 | ETH_REG))
#define EPMM3 ((uint8_t)(0x0B | BANK_1 | ETH_REG))
#define EPMM4 ((uint8_t)(0x0C | BANK_1 | ETH_REG))
#define EPMM5 ((uint8_t)(0x0D | BANK_1 | ETH_REG))
#define EPMM6 ((uint8_t)(0x0E | BANK_1 | ETH_REG))
#define EPMM7 ((uint8_t)(0x0F | BANK_1 | ETH_REG))
#define EPMCS ((uint8_t)(0x10 | BANK_1 | ETH_REG))
#define EPMO ((uint8_t)(0x14 | BANK_1 | ETH_REG))
#define ERXFCON ((uint8_t)(0x18 | BANK_1 | ETH_REG))
#define EPKTCNT ((uint8_t)(0x19 | BANK_1 | ETH_REG))

//Bank 2 Regs

#define MACON1 ((uint8_t)(0x00 | BANK_2))
#define MACON3 ((uint8_t)(0x02 | BANK_2))
#define MACON4 ((uint8_t)(0x03 | BANK_2))
#define MABBIPG ((uint8_t)(0x04 | BANK_2))
#define MAIPG ((uint8_t)(0x06 | BANK_2))
#define MACLCON1 ((uint8_t)(0x08 | BANK_2))
#define MACLCON2 ((uint8_t)(0x09 | BANK_2))
#define MAMXFL ((uint8_t)(0x0A | BANK_2))
#define MICMD ((uint8_t)(0x12 | BANK_2))
#define MIREGADR ((uint8_t)(0x14 | BANK_2))
#define MIWR ((uint8_t)(0x16 | BANK_2))
#define MIRD ((uint8_t)(0x18 | BANK_2))


//Bank 3 Regs

#define MAADR5 ((uint8_t)(0x00 | BANK_3))
#define MAADR6 ((uint8_t)(0x01 | BANK_3))
#define MAADR3 ((uint8_t)(0x02 | BANK_3))
#define MAADR4 ((uint8_t)(0x03 | BANK_3))
#define MAADR1 ((uint8_t)(0x04 | BANK_3))
#define MAADR2 ((uint8_t)(0x05 | BANK_3))
#define EBSTSD ((uint8_t)(0x06 | BANK_3 | ETH_REG))
#define EBSTCON ((uint8_t)(0x07 | BANK_3 | ETH_REG))
#define EBSTCS ((uint8_t)(0x08 | BANK_3 | ETH_REG))
#define MISTAT ((uint8_t)(0x0A | BANK_3))
#define EREVID ((uint8_t)(0x12 | BANK_3 | ETH_REG))
#define ECOCON ((uint8_t)(0x15 | BANK_3 | ETH_REG))
#define EFLOCON ((uint8_t)(0x17 | BANK_3 | ETH_REG))
#define EPAUS ((uint8_t)(0x18 | BANK_3 | ETH_REG))

// Physical Regs

#define PHCON1 ((uint8_t)0x00)
#define PHSTAT1 ((uint8_t)0x01)
#define PHID1 ((uint8_t)0x02)
#define PHID2 ((uint8_t)0x03)
#define PHCON2 ((uint8_t)0x10)
#define PHSTAT2 ((uint8_t)0x11)
#define PHIE ((uint8_t)0x12)
#define PHIR ((uint8_t)0x13)
#define PHLCON ((uint8_t)0x14)


// ECON1 bits

#define ECON1_BANK0 ((uint8_t)0x00)
#define ECON1_BANK1 ((uint8_t)0x01)
#define ECON1_BANK2 ((uint8_t)0x02)
#define ECON1_BANK3 ((uint8_t)0x03)
#define ECON1_BANK ((uint8_t)0x03)

#define ECON1_RXEN ((uint8_t)0x04)
#define ECON1_TXRTS ((uint8_t)0x08)
#define ECON1_CSUMEN ((uint8_t)0x10)
#define ECON1_DMAST ((uint8_t)0x20)
#define ECON1_RXRST ((uint8_t)0x40)
#define ECON1_TXRST ((uint8_t)0x80)

// ECON2 bits

#define ECON2_VRPS ((uint8_t)0x08)
#define ECON2_PWRSV ((uint8_t)0x20)
#define ECON2_PKTDEC ((uint8_t)0x40)
#define ECON2_AUTOINC ((uint8_t)0x80)

// MICMD bits

#define MICMD_MIIRD ((uint8_t)0x01)
#define MICMD_MIISCAN ((uint8_t)0x02)

// MISTAT bits

#define MISTAT_BUSY ((uint8_t)0x01)
#define MISTAT_SCAN ((uint8_t)0x02)
#define MISTAT_NVALID ((uint8_t)0x04)

// PHSTAT1 bits

#define PHSTAT1_JBSTAT ((uint16_t)0x0002)
#define PHSTAT1_LLSTAT ((uint16_t)0x0004)
#define PHSTAT1_PHDPX ((uint16_t)0x0800)
#define PHSTAT1_PFDPX ((uint16_t)0x1000)

// PHSTAT2 bits

#define PHSTAT2_PLRITY ((uint16_t)0x0020)
#define PHSTAT2_DPXSTAT ((uint16_t)0x0200)
#define PHSTAT2_LSTAT ((uint16_t)0x0400)
#define PHSTAT2_COLSTAT ((uint16_t)0x0800)
#define PHSTAT2_RXSTAT ((uint16_t)0x1000)
#define PHSTAT2_TXSTAT ((uint16_t)0x2000)

// PHCON1 bits

#define PHCON1_PDPXMD ((uint16_t)0x0100)
#define PHCON1_PPWRSV ((uint16_t)0x0800)
#define PHCON1_PLOOPBK ((uint16_t)0x4000)
#define PHCON1_PRST ((uint16_t)0x8000)


// PHCON2 bits

#define PHCON2_HDLDIS ((uint16_t)0x0100)
#define PHCON2_JABBER ((uint16_t)0x0400)
#define PHCON2_TXDIS ((uint16_t)0x2000)
#define PHCON2_FRCLNK ((uint16_t)0x4000)

// PHIE bits

#define PHIE_PGEIE ((uint16_t)0x0002)
#define PHIE_PLNKIE ((uint16_t)0x0010)

// PHIR bits

#define PHIR_PGIF ((uint16_t)0x0004)
#define PHIR_PLNKIF ((uint16_t)0x0010)

// PHLCON bits
#define PHLCON_STRCH ((uint16_t)0x0002)
#define PHLCON_LFRQ_0 ((uint16_t)0x0004)
#define PHLCON_LFRQ_1 ((uint16_t)0x0008)
#define PHLCON_LFRQ ((uint16_t)0x000C)
#define PHLCON_LBCFG_0 ((uint16_t)0x0010)
#define PHLCON_LBCFG_1 ((uint16_t)0x0020)
#define PHLCON_LBCFG_2 ((uint16_t)0x0040)
#define PHLCON_LBCFG_3 ((uint16_t)0x0080)
#define PHLCON_LBCFG ((uint16_t)0x00F0)
#define PHLCON_LACFG_0 ((uint16_t)0x0100)
#define PHLCON_LACFG_1 ((uint16_t)0x0200)
#define PHLCON_LACFG_2 ((uint16_t)0x0400)
#define PHLCON_LACFG_3 ((uint16_t)0x0800)
#define PHLCON_LACFG ((uint16_t)0x0F00)

// MACON1 bits

#define MACON1_MARXEN ((uint8_t)0x01)
#define MACON1_PASSALL ((uint8_t)0x02)
#define MACON1_RXPAUS ((uint8_t)0x04)
#define MACON1_TXPAUS ((uint8_t)0x08)

// MACON3 bits

#define MACON3_FULDPX ((uint8_t)0x01)
#define MACON3_FRMLNEN ((uint8_t)0x02)
#define MACON3_HFRMEN ((uint8_t)0x04)
#define MACON3_PHDREN ((uint8_t)0x08)
#define MACON3_TXCRCEN ((uint8_t)0x10)
#define MACON3_PADCFG_0 ((uint8_t)0x20)
#define MACON3_PADCFG_1 ((uint8_t)0x40)
#define MACON3_PADCFG_2 ((uint8_t)0x80)
#define MACON3_PADCFG ((uint8_t)0xE0)

#define MACON3_PADCFG_60BITS_CRC MACON3_PADCFG_0

// MACON4 bits

#define MACON4_NOBKOFF ((uint8_t)0x10)
#define MACON4_BPEN ((uint8_t)0x20)
#define MACON4_DEFER ((uint8_t)0x40)

// ERXFCON bits

#define ERXFCON_BCEN ((uint8_t)0x01)
#define ERXFCON_MCEN ((uint8_t)0x02)
#define ERXFCON_HTEN ((uint8_t)0x04)
#define ERXFCON_MPEN ((uint8_t)0x08)
#define ERXFCON_PMEN ((uint8_t)0x10)
#define ERXFCON_CRCEN ((uint8_t)0x20)
#define ERXFCON_ANDOR ((uint8_t)0x40)
#define ERXFCON_UCEN ((uint8_t)0x80)

// EFLOCON bits

#define EFLOCON_FCEN_0 ((uint8_t)0x01)
#define EFLOCON_FCEN_1 ((uint8_t)0x02)
#define EFLOCON_FCEN ((uint8_t)0x03)
#define EFLOCON_FULDPXS ((uint8_t)0x04)


// ESTAT bits

#define ESTAT_CLKRDY ((uint8_t)0x01)
#define ESTAT_TXABRT ((uint8_t)0x02)
#define ESTAT_RXBUSY ((uint8_t)0x04)
#define ESTAT_LATECOL ((uint8_t)0x10)
#define ESTAT_BUFER ((uint8_t)0x40)
#define ESTAT_INT ((uint8_t)0x80)

// EIE bits

#define EIE_RXERIE ((uint8_t)0x01)
#define EIE_TXERIE ((uint8_t)0x02)
#define EIE_TXIE ((uint8_t)0x08)
#define EIE_LINKIE ((uint8_t)0x10)
#define EIE_DMAIE ((uint8_t)0x20)
#define EIE_PKTIE ((uint8_t)0x40)
#define EIE_INTIE ((uint8_t)0x80)

// EIR bits

#define EIR_RXERIF ((uint8_t)0x01)
#define EIR_TXERIF ((uint8_t)0x02)
#define EIR_TXIF ((uint8_t)0x08)
#define EIR_LINKIF ((uint8_t)0x10)
#define EIR_DMAIF ((uint8_t)0x20)
#define EIR_PKTIF ((uint8_t)0x40)

// EBSTCON bits

#define EBSTCON_BISTST ((uint8_t)0x01)
#define EBSTCON_TME ((uint8_t)0x02)
#define EBSTCON_TMSEL_0 ((uint8_t)0x04)
#define EBSTCON_TMSEL_1 ((uint8_t)0x08)
#define EBSTCON_TMSEL ((uint8_t)0x0C)
#define EBSTCON_PSEL ((uint8_t)0x10)
#define EBSTCON_PSV_0 ((uint8_t)0x20)
#define EBSTCON_PSV_1 ((uint8_t)0x40)
#define EBSTCON_PSV_2 ((uint8_t)0x80)
#define EBSTCON_PSV ((uint8_t)0xE0)

// ECOCON bits
#define ECOCON_COCON_0 ((uint8_t)0x01)
#define ECOCON_COCON_1 ((uint8_t)0x02)
#define ECOCON_COCON_2 ((uint8_t)0x04)
#define ECOCON_COCON ((uint8_t)0x07)




// Opcodes

#define RD_CON_REG ((uint8_t)0x00)
#define RD_BUF_MEM ((uint8_t)0x3A)
#define WR_CON_REG ((uint8_t)0x40)
#define WR_BUF_MEM ((uint8_t)0x7A)
#define BFS ((uint8_t)0x80)
#define BFC ((uint8_t)0xA0)
#define SOFT_RES ((uint8_t)0xFF)



void spi_transmit_receive(uint8_t *data_trmt, uint8_t *data_rec, uint16_t size);
uint8_t spi_write_read(uint8_t wr_byte);
void spi_send_byte(uint8_t wr_byte);
uint8_t spi_receive_byte(void);
void enc28_write_op(uint8_t opcode, uint8_t address, uint8_t data);
uint8_t enc28_read_op(uint8_t opcode, uint8_t address);
void enc28_set_bank(uint8_t reg_addr);
void enc28_write_reg(uint8_t address, uint8_t data);
uint8_t enc28_read_reg(uint8_t address);
void enc28_write_regs_two_parts(uint8_t address, uint16_t data);
uint16_t enc28_read_regs_two_parts(uint8_t address);
void enc28_write_phy_reg(uint8_t address, uint16_t data);
uint16_t enc28_read_phy_reg(uint8_t address);
void enc28_receive_buff(uint8_t data[], uint16_t len);
void enc28_transmit_buff(uint8_t data[], uint16_t len);
uint16_t enc28_packet_receive(enc28_frame *frame_rec);
void enc28_packet_transmit(enc28_frame *frame_repl);
void enc28_init(void);


#endif

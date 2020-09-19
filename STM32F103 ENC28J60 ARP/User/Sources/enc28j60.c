#include "stm32f10x.h"
#include "gpio.h"
#include "enc28j60.h"
#include "spi.h"
#include "uart.h"
#include "tims.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


uint8_t mac_addr[6] = {0x00,0x15,0x42,0xBF,0xF0,0x51};
uint8_t mac_addr_null[6] = {0x00,0x00,0x00,0x00,0x00,0x00};
uint8_t mac_addr_bc[6] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
uint8_t ip_addr[4] = {192,168,0,2};
uint8_t ip_serv[4];
enc28_frame frame;

uint16_t next_pckt_ptr = RX_ST;
uint8_t Enc28j60Bank;


void spi_transmit_receive(uint8_t *data_trmt, uint8_t *data_rec, uint16_t size)
{
	SPI1->CR1 |=  SPI_CR1_SPE;
	
	while(size--)
	{
		SPI1->DR = *data_trmt++;
		while(!(SPI1->SR & SPI_SR_TXE))
			;

		while(!(SPI1->SR & SPI_SR_RXNE))
			;
		*data_rec++ = SPI1->DR;
		
	}
	while(SPI1->SR & SPI_SR_BSY)
		;
	
	SPI1->CR1 &=  ~SPI_CR1_SPE;
	
	
}

uint8_t spi_write_read(uint8_t wr_byte)
{
	uint8_t rd_byte;
	
	spi_transmit_receive(&wr_byte, &rd_byte, 1);
	
	return rd_byte;
	
}

void spi_send_byte(uint8_t wr_byte)
{
	spi_write_read(wr_byte);
}

uint8_t spi_receive_byte(void)
{
	uint8_t rec_byte = spi_write_read(0);
	
	return rec_byte;
}

void enc28_write_op(uint8_t opcode, uint8_t address, uint8_t data)
{	
	nss_sel();
	
	spi_send_byte(opcode | (address & ENC28_ADDR_MASK));
	spi_send_byte(data);

	nss_desel();
}

uint8_t enc28_read_op(uint8_t opcode, uint8_t address)
{
	uint8_t res;
	
	nss_sel();
	
	spi_send_byte(opcode | (address & ENC28_ADDR_MASK));
	if(!(address & ETH_REG))
		spi_receive_byte();
	
	res = spi_receive_byte();
	
	nss_desel();
	return res;
}

void enc28_set_bank(uint8_t reg_addr)
{
	if((reg_addr & BANK_REG_MASK) != Enc28j60Bank)
	{
		Enc28j60Bank = reg_addr & BANK_REG_MASK;
		enc28_write_op(BFC, ECON1, ECON1_BANK);
		enc28_write_op(BFS, ECON1, Enc28j60Bank >> 5);
	}
	
}


void enc28_write_reg(uint8_t address, uint8_t data)
{
	enc28_set_bank(address);
	enc28_write_op(WR_CON_REG, address, data);
}


uint8_t enc28_read_reg(uint8_t address)
{
	uint8_t res;
	
	enc28_set_bank(address);
	res = enc28_read_op(RD_CON_REG, address);
	
	return res;
}


void enc28_write_regs_two_parts(uint8_t address, uint16_t data)
{
	enc28_write_reg(address, (uint8_t)data);
	enc28_write_reg(address+1, (uint8_t)(data >> 8));
}

uint16_t enc28_read_regs_two_parts(uint8_t address)
{
	uint16_t res;
	
	res = enc28_read_reg(address);
	res |= ((uint16_t)enc28_read_reg(address+1) << 8);
	
	return res;
	
}

void enc28_write_phy_reg(uint8_t address, uint16_t data)
{
	enc28_write_reg(MIREGADR, address);
	enc28_write_regs_two_parts(MIWR, data);
	
	while(enc28_read_reg(MISTAT) & MISTAT_BUSY)
		;
}

uint16_t enc28_read_phy_reg(uint8_t address)
{
	uint8_t micmd;
	
	enc28_write_reg(MIREGADR, address);
	enc28_write_reg(MICMD, MICMD_MIIRD);	
	
	while(enc28_read_reg(MISTAT) & MISTAT_BUSY)
		;
	
	micmd = enc28_read_reg(MICMD);
	micmd &= ~MICMD_MIIRD;
	enc28_write_reg(MICMD, micmd);
	
	return enc28_read_regs_two_parts(MIRD);
}

void enc28_receive_buff(uint8_t data[], uint16_t len)
{
	nss_sel();
	
	spi_send_byte(RD_BUF_MEM);
	
	while(len--)
		*data++ = spi_receive_byte();
	
	nss_desel();	
}

void enc28_transmit_buff(uint8_t data[], uint16_t len)
{
	nss_sel();
	
	spi_send_byte(WR_BUF_MEM);
	
	while(len--)
		spi_send_byte(*data++);
	
	nss_desel();
	
}


void enc28_init(void)
{
	uint16_t reg16;
	uint8_t reg8;
	
	enc28_write_op(SOFT_RES, SOFT_RES, 0);
	delay_us(4000);
	while(!(enc28_read_op(RD_CON_REG, ESTAT) & ESTAT_CLKRDY))
		;
	
	// Receive Buffer Configuration
	enc28_write_regs_two_parts(ERXST, RX_ST);
	enc28_write_regs_two_parts(ERXRDPT, RX_ST);
	enc28_write_regs_two_parts(ERXND, RX_ND);
	reg16 = enc28_read_regs_two_parts(ERXND);
	enc28_write_regs_two_parts(ETXST, TX_ST);
	enc28_write_regs_two_parts(ETXND, TX_ND);

	
	// Receive Filter Configuration
	enc28_set_bank(ERXFCON);
	enc28_write_op(BFS, ERXFCON, (ERXFCON_UCEN | ERXFCON_CRCEN | ERXFCON_BCEN));
	reg8 = enc28_read_reg(ERXFCON);
	
	// MAC Initialization
	enc28_write_reg(MACON1, (MACON1_MARXEN | MACON1_RXPAUS | MACON1_TXPAUS));
	enc28_write_reg(MACON3, (MACON3_PADCFG_60BITS_CRC | MACON3_FULDPX | MACON3_TXCRCEN | MACON3_FRMLNEN));
	enc28_write_regs_two_parts(MAMXFL, MAX_FRAMELEN);
	enc28_write_reg(MABBIPG, 0x12);
	reg8 = enc28_read_reg(MABBIPG);
	enc28_write_regs_two_parts(MAIPG, 0x0C12);
	reg16 = enc28_read_regs_two_parts(MAIPG);
	
	//MAC Address Initialization
	enc28_write_reg(MAADR1, mac_addr[0]);
	enc28_write_reg(MAADR2, mac_addr[1]);
	enc28_write_reg(MAADR3, mac_addr[2]);
	enc28_write_reg(MAADR4, mac_addr[3]);
	enc28_write_reg(MAADR5, mac_addr[4]);
	enc28_write_reg(MAADR6, mac_addr[5]);
	reg8 = enc28_read_reg(MAADR6);
	
	// Phy Regs Initialization
	enc28_write_phy_reg(PHCON1, PHCON1_PDPXMD);
	reg16 = enc28_read_phy_reg(PHCON1);
	enc28_write_phy_reg(PHCON2, PHCON2_HDLDIS); 
	enc28_write_phy_reg(PHLCON,PHLCON_LACFG_2 | PHLCON_LBCFG_2 |PHLCON_LBCFG_1 | PHLCON_LBCFG_0 | PHLCON_LFRQ_0 |PHLCON_STRCH);
	
	enc28_write_op(BFS, ECON2, ECON2_AUTOINC);
	
	// Receive enable
	enc28_set_bank(ECON1);
	enc28_write_op(BFS, EIE, EIE_PKTIE | EIE_INTIE);
	enc28_write_op(BFC, EIR, EIR_PKTIF);
	enc28_write_op(BFS, ECON1, ECON1_RXEN);
	
}




uint16_t enc28_packet_receive(enc28_frame *frame_rec)
{
	uint16_t len = 0;
	
	if(enc28_read_reg(EPKTCNT) > 0)
	{
		enc28_write_regs_two_parts(ERDPT, next_pckt_ptr);
		
		struct
		{
			uint16_t next_pckt;
			uint16_t byte_cnt;
			uint16_t status;
		} header;
		enc28_receive_buff((uint8_t *)&header, sizeof(header));
		next_pckt_ptr = header.next_pckt;
		len = header.byte_cnt - 4 - sizeof(enc28_frame_header); // exclude CRC and header
		if(len > ENC28J60_MAXFRAME)
			len = ENC28J60_MAXFRAME;
		if((header.status & 0x80) == 0)
			len = 0;
		else
		{
			frame_rec->data_len = len;
			enc28_receive_buff((uint8_t *)&frame_rec->hdr, sizeof(enc28_frame_header));
			frame_rec->data = (uint8_t *) malloc((frame_rec->data_len) * sizeof(uint8_t));
			enc28_receive_buff(frame_rec->data, frame_rec->data_len);
		}

		
		if(next_pckt_ptr - 1 > RX_ND)
			enc28_write_regs_two_parts(ERXRDPT, RX_ND);
		else
			enc28_write_regs_two_parts(ERXRDPT, next_pckt_ptr - 1);
		
		enc28_write_op(BFS, ECON2, ECON2_PKTDEC);
		
	}
	return len;

}





void enc28_packet_transmit(enc28_frame *frame_repl)
{
	uint8_t pcr = 0;
	
	enc28_write_regs_two_parts(ETXST, TX_ST);
	enc28_write_regs_two_parts(EWRPT, TX_ST);
	enc28_write_regs_two_parts(ETXND, TX_ST + sizeof(enc28_frame_header) + frame_repl->data_len);
	
	enc28_transmit_buff(&pcr, 1);
	enc28_transmit_buff((uint8_t*)&frame_repl->hdr, sizeof(enc28_frame_header));
	enc28_transmit_buff((uint8_t*)frame_repl->data, frame_repl->data_len);
	
	enc28_write_op(BFS, ECON1, ECON1_TXRTS);
	while(enc28_read_reg(ECON1) & ECON1_TXRTS)
		;
	
	
}





/*
 * display.c
 *
 *  Created on: 20 апр. 2026 г.
 *      Author: georg
 */

#include "display.h"

uint8_t numbers[10] = { 0b1111110, 0b0110000, 0b1101101, 0b1111001, 0b0110011, 0b1011011, 0b1011111, 0b1110000, 0b1111111, 0b1111011 };

void display_send_data(uint16_t addr, uint16_t data)
{
	uint16_t final = (addr << 8) | data;

	CS_LOW

	SPI1->DR = final;

	while(SPI1->SR & SPI_SR_BSY) asm("nop");

	for(int i = 0; i < 10; i++) asm("nop");

	CS_HIGH
}

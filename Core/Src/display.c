/*
 * display.c
 *
 *  Created on: 20 ���. 2026 �.
 *      Author: georg
 */

#include "display.h"
#include "spi.h"


#define CS_LOW()  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET)
#define CS_HIGH() HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET)

uint8_t numbers[10] = {
    0b1111110,
    0b0110000,
    0b1101101,
    0b1111001,
    0b0110011,
    0b1011011,
    0b1011111,
    0b1110000,
    0b1111111,
    0b1111011
};

void display_send_data(uint16_t addr, uint16_t data)
{
    uint16_t tx = (addr << 8) | data;

    CS_LOW();

    HAL_SPI_Transmit(&hspi1, (uint8_t *)&tx, 1, HAL_MAX_DELAY);

    CS_HIGH();
}

//#define CS_LOW GPIOA->BSRR = GPIO_BSRR_BR_4;
//#define CS_HIGH GPIOA->BSRR = GPIO_BSRR_BS_4;
//
//
//uint8_t numbers[10] = { 0b1111110, 0b0110000, 0b1101101, 0b1111001, 0b0110011, 0b1011011, 0b1011111, 0b1110000, 0b1111111, 0b1111011 };
//
//void display_send_data(uint16_t addr, uint16_t data)
//{
//	uint16_t final = (addr << 8) | data;
//
//	CS_LOW
//
//	SPI1->DR = final;
//
//	while(SPI1->SR & SPI_SR_BSY) asm("nop");
//
//	for(int i = 0; i < 10; i++) asm("nop");
//
//	CS_HIGH
//}

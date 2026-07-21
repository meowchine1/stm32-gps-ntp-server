/*
 * w5500_port.c
 *
 *  Created on: Jul 17, 2026
 *      Author: katev
 */


#include "w5500_port.h"
#include "wizchip_conf.h"
#include "stm32f4xx.h"
#include "spi.h"
#include "main.h"

static uint8_t rx_buffer[100];
static uint8_t tx_buffer[100];

void wizchip_select(void)
{
    HAL_GPIO_WritePin(w5500_CS_GPIO_Port, w5500_CS_Pin, GPIO_PIN_RESET);
}

void wizchip_deselect(void)
{
    HAL_GPIO_WritePin(w5500_CS_GPIO_Port, w5500_CS_Pin, GPIO_PIN_SET);
}


uint8_t wizchip_read_byte(void)
{
    uint8_t tx = 0xFF;
    uint8_t rx = 0x00;

    HAL_SPI_TransmitReceive(&hspi2, &tx, &rx, 1, HAL_MAX_DELAY);

    return rx;
}


void wizchip_write_byte(uint8_t data)
{
    HAL_SPI_Transmit(&hspi2, &data, 1, HAL_MAX_DELAY);
}


void w5500_io_init(void)
{

    reg_wizchip_cs_cbfunc(
        wizchip_select,
        wizchip_deselect
    );


    reg_wizchip_spi_cbfunc(
        wizchip_read_byte,
        wizchip_write_byte
    );


}

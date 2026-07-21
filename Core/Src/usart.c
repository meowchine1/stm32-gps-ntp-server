/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.c
  * @brief   This file provides code for the configuration
  *          of the USART instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/*
 *  Created on: 20 апр. 2026 г.
 *      Author: georg
 */

/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "usart.h"

/* USER CODE BEGIN 0 */

#include "ring_buf.h"
#include "watchdog.h"
#include "display.h"
#include "string.h"

/* USER CODE END 0 */

UART_HandleTypeDef huart1;

/* USART1 init function */

void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspInit 0 */

  /* USER CODE END USART1_MspInit 0 */
    /* USART1 clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART1 interrupt Init */
    HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspInit 1 */

  /* USER CODE END USART1_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspDeInit 0 */

  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();

    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);

    /* USART1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspDeInit 1 */

  /* USER CODE END USART1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

// ****************** Georg code **************************************************************************************************

uart_parser_t uart_parser;
extern RingBuffer uart_rx_buf;
extern uint8_t numbers[];

uint8_t hour;
uint8_t minute;
uint8_t second;

const char packet_header_nmea_gps[] = "$GPRMC,";
const char packet_header_nmea_glonass[] = "$GLRMC,";
const char packet_header_nmea_galileo[] = "$GARMC,";
const char packet_header_nmea_beidou_1[] = "$GBRMC,";
const char packet_header_nmea_beidou_2[] = "$BDRMC,";
const char packet_header_nmea_qzss[] = "$GQRMC,";
const char packet_header_nmea_gnss[] = "$GNRMC,";

void reset_uart_parser(void)
{
	memset(&uart_parser, 0, sizeof(uart_parser_t));
}


void process_uart(void)
{
	uint8_t byte;
	uint8_t error = buffer_get_from_front(&uart_rx_buf, &byte);

	if(error) return;

	switch(uart_parser.state)
	{
		case STATE_BEGIN:
		{
			if(byte == 0x24) //'$'
			{
				uart_parser.state = STATE_READ_PACKET;
				uart_parser.ntp = 0; //nmea
				uart_parser.buffer[uart_parser.curr_byte] = byte;
				uart_parser.curr_byte++;

				start_watchdog();
				break;
			}
			break;
		}

		case STATE_READ_PACKET:
		{
			if(uart_parser.ntp == 0) //nmea
			{
				uart_parser.buffer[uart_parser.curr_byte] = byte;
				uart_parser.curr_byte++;
				bump_watchdog();

				if(uart_parser.curr_byte == 13) //HACK: we read first part of the packet where time should be
				{
					if(memcmp((void*)&uart_parser.buffer[0], (void*)&packet_header_nmea_gps, 6) == 0 ||
							memcmp((void*)&uart_parser.buffer[0], (void*)&packet_header_nmea_glonass, 6) == 0 ||
							memcmp((void*)&uart_parser.buffer[0], (void*)&packet_header_nmea_galileo, 6) == 0 ||
							memcmp((void*)&uart_parser.buffer[0], (void*)&packet_header_nmea_beidou_1, 6) == 0 ||
							memcmp((void*)&uart_parser.buffer[0], (void*)&packet_header_nmea_beidou_2, 6) == 0 ||
							memcmp((void*)&uart_parser.buffer[0], (void*)&packet_header_nmea_qzss, 6) == 0 ||
							memcmp((void*)&uart_parser.buffer[0], (void*)&packet_header_nmea_gnss, 6) == 0) //valid packet? we don't read full packet and do checksum check
					{
						stop_watchdog();

						hour = (uart_parser.buffer[7] - 0x30) * 10 + (uart_parser.buffer[8] - 0x30); // 0x30 = '0'
						minute = (uart_parser.buffer[9] - 0x30) * 10 + (uart_parser.buffer[10] - 0x30);
						second = (uart_parser.buffer[11] - 0x30) * 10 + (uart_parser.buffer[12] - 0x30);

						sync_time_from_gps(hour, minute, second);

						display_send_data(3, DASH);
						display_send_data(6, DASH);

						display_send_data(2, numbers[second / 10]); //seconds
						display_send_data(1, numbers[second % 10]);

						display_send_data(5, numbers[minute / 10]); //minutes
						display_send_data(4, numbers[minute % 10]);

						display_send_data(8, numbers[hour / 10]); //hours
						display_send_data(7, numbers[hour % 10]);

						reset_uart_parser();
						break;
					}
					else
					{
						stop_watchdog();
						reset_uart_parser();
						break;
					}
				}
			}

			break;
		}

		default: break;
	}
}

void interrupt_uart_processor(void){

	//extern UART_HandleTypeDef huart1;
	if (USART1->SR & USART_SR_RXNE){
		//GPIOA->BSRR = GPIO_BSRR_BS_0;

		buffer_put_to_end(&uart_rx_buf,
				(USART1->DR == 0xB6 || USART1->DR == 0xA6) ?
						0x24 : ((USART1->DR == 0xB4 || USART1->DR == 0xA4) ?
								0x24 : USART1->DR));

		if(USART1->DR == 0x24) //'$'
		{
			reset_uart_parser();
		}

		//USART2->DR = USART1->DR;
		//GPIOA->BSRR = GPIO_BSRR_BR_0;
	}
}



/* USER CODE END 1 */

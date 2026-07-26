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
#include "time.h"
#include <string.h>
#include <stdlib.h>

/* USER CODE END 0 */

/* USART1 init function */

void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  LL_USART_InitTypeDef USART_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* Peripheral clock enable */
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART1);

  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
  /**USART1 GPIO Configuration
  PA9   ------> USART1_TX
  PA10   ------> USART1_RX
  */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_9|LL_GPIO_PIN_10;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_7;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USART1 DMA Init */

  /* USART1_RX Init */
  LL_DMA_SetChannelSelection(DMA2, LL_DMA_STREAM_2, LL_DMA_CHANNEL_4);

  LL_DMA_SetDataTransferDirection(DMA2, LL_DMA_STREAM_2, LL_DMA_DIRECTION_PERIPH_TO_MEMORY);

  LL_DMA_SetStreamPriorityLevel(DMA2, LL_DMA_STREAM_2, LL_DMA_PRIORITY_MEDIUM);

  LL_DMA_SetMode(DMA2, LL_DMA_STREAM_2, LL_DMA_MODE_NORMAL);

  LL_DMA_SetPeriphIncMode(DMA2, LL_DMA_STREAM_2, LL_DMA_PERIPH_NOINCREMENT);

  LL_DMA_SetMemoryIncMode(DMA2, LL_DMA_STREAM_2, LL_DMA_MEMORY_INCREMENT);

  LL_DMA_SetPeriphSize(DMA2, LL_DMA_STREAM_2, LL_DMA_PDATAALIGN_BYTE);

  LL_DMA_SetMemorySize(DMA2, LL_DMA_STREAM_2, LL_DMA_MDATAALIGN_BYTE);

  LL_DMA_DisableFifoMode(DMA2, LL_DMA_STREAM_2);

  /* USART1 interrupt Init */
  NVIC_SetPriority(USART1_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
  NVIC_EnableIRQ(USART1_IRQn);

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  USART_InitStruct.BaudRate = 4800;
  USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
  USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
  USART_InitStruct.Parity = LL_USART_PARITY_NONE;
  USART_InitStruct.TransferDirection = LL_USART_DIRECTION_RX;
  USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
  USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
  LL_USART_Init(USART1, &USART_InitStruct);
  LL_USART_ConfigAsyncMode(USART1);
  LL_USART_Enable(USART1);
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/* USER CODE BEGIN 1 */

// ****************** Georg code **************************************************************************************************

uart_parser_t uart_parser;
extern RingBuffer uart_rx_buf;
extern uint8_t numbers[];
uint8_t hour;
uint8_t minute;
uint8_t second;
uint16_t year;
uint8_t month;
uint8_t day;

//uint8_t hour;
//uint8_t minute;
//uint8_t second;

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


void parse_rmc_packet(char *packet)
{

    char *token;
    uint8_t field = 0;

    token = strtok(packet, ",");

    while(token != NULL)
    {
        switch(field)
        {
            case 1: // UTC time hhmmss.ss
            {
                if(strlen(token) >= 6)
                {
                    hour =
                        (token[0] - '0') * 10 +
                        (token[1] - '0');

                    minute =
                        (token[2] - '0') * 10 +
                        (token[3] - '0');

                    second =
                        (token[4] - '0') * 10 +
                        (token[5] - '0');
                }

                break;
            }


            case 9: // date ddmmyy
            {
                if(strlen(token) == 6)
                {
                    day =
                        (token[0] - '0') * 10 +
                        (token[1] - '0');

                    month =
                        (token[2] - '0') * 10 +
                        (token[3] - '0');

                    year =
                        2000 +
                        (token[4] - '0') * 10 +
                        (token[5] - '0');
                }

                break;
            }
        }

        token = strtok(NULL, ",");
        field++;
    }

    sync_time_from_gps(
        year,
        month,
        day,
        hour,
        minute,
        second
    );
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
			if(byte == 0x24) // if find '$'
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

//						hour = (uart_parser.buffer[7] - 0x30) * 10 + (uart_parser.buffer[8] - 0x30); // 0x30 = '0'
//						minute = (uart_parser.buffer[9] - 0x30) * 10 + (uart_parser.buffer[10] - 0x30);
//						second = (uart_parser.buffer[11] - 0x30) * 10 + (uart_parser.buffer[12] - 0x30);
//
//						sync_time_from_gps(hour, minute, second);

						parse_rmc_packet((char*)uart_parser.buffer);

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

//void interrupt_uart_processor(void){
//
//	//extern UART_HandleTypeDef huart1;
//	if (USART1->SR & USART_SR_RXNE){
//		//GPIOA->BSRR = GPIO_BSRR_BS_0;
//
//		buffer_put_to_end(&uart_rx_buf,
//				(USART1->DR == 0xB6 || USART1->DR == 0xA6) ?
//						0x24 : ((USART1->DR == 0xB4 || USART1->DR == 0xA4) ?
//								0x24 : USART1->DR));
//
//		if(USART1->DR == 0x24) //'$'
//		{
//			reset_uart_parser();
//		}
//
//		//USART2->DR = USART1->DR;
//		//GPIOA->BSRR = GPIO_BSRR_BR_0;
//	}
//}



/* USER CODE END 1 */

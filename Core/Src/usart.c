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

#define UART_BUF_SIZE 120 // Убедитесь, что размер uart_parser.buffer достаточно велик (например, 100 байт)
volatile uint8_t is_time_synced = 0;
extern volatile uint32_t uart_loss_timeout_counter;

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
  huart1.Init.BaudRate = 4800;
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

uart_parser_t uart_parser;
extern RingBuffer uart_rx_buf;
extern uint8_t numbers[];
uint8_t hour;
uint8_t minute;
uint8_t second;
uint16_t year;
uint8_t month;
uint8_t day;

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
    // Защита: если спутники не пойманы (символ 'V'), не парсим мусор
    if (strstr(packet, ",V,") != NULL)
    {
        return;
    }

    char *token;
    uint8_t field = 0;

    // Временные переменные, чтобы ничего не накладывалось друг на друга
    uint8_t rmc_hour = 0, rmc_minute = 0, rmc_second = 0;
    uint8_t rmc_day = 0, rmc_month = 0;
    uint16_t rmc_year = 0;

    token = strtok(packet, ",");

    while(token != NULL)
    {
        switch(field)
        {
            case 1: // UTC time (hhmmss.ss)
            {
                if(strlen(token) >= 6)
                {
                    rmc_hour   = (token[0] - '0') * 10 + (token[1] - '0');
                    rmc_minute = (token[2] - '0') * 10 + (token[3] - '0');
                    rmc_second = (token[4] - '0') * 10 + (token[5] - '0');
                }
                break;
            }

            case 9: // Date (ddmmyy)
            {
                if(strlen(token) == 6)
                {
                    rmc_day   = (token[0] - '0') * 10 + (token[1] - '0');
                    rmc_month = (token[2] - '0') * 10 + (token[3] - '0');
                    rmc_year  = 2000 + (token[4] - '0') * 10 + (token[5] - '0');
                }
                break;
            }
        }

        token = strtok(NULL, ",");
        field++;
    }

    // Если дата успешно распарсилась и она валидна (год >= 2024)
    if(rmc_year >= 2024)
    {
        // 1. Обновляем глобальные переменные для вашего дисплея платы
        hour   = rmc_hour;
        minute = rmc_minute;
        second = rmc_second;
        day    = rmc_day;
        month  = rmc_month;
        year   = rmc_year;

        // 2. Запускаем расчет секунд NTP от 1900 года
        sync_time_from_gps(
            year,
            month,
            day,
            hour,
            minute,
            second
        );
        // ВРЕМЯ УСПЕШНО ПОЛУЧЕНО: Выставляем флаг синхронизации!
        is_time_synced = 1;
        uart_loss_timeout_counter = 0; // СБРАСЫВАЕМ ТАЙМАУТ ПОТЕРI СВЯЗИ! Скрипт на связи.

    }
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
            if(byte == 0x24) // '$' - начало NMEA пакета
            {
                uart_parser.state = STATE_READ_PACKET;
                uart_parser.ntp = 0;
                uart_parser.curr_byte = 0; // Сбрасываем индекс на 0 перед записью
                uart_parser.buffer[uart_parser.curr_byte++] = byte;

                start_watchdog();
            }
            break;
        }

        case STATE_READ_PACKET:
        {
            // Защита от переполнения буфера
            if (uart_parser.curr_byte >= (UART_BUF_SIZE - 1)) {
                stop_watchdog();
                reset_uart_parser();
                break;
            }

            uart_parser.buffer[uart_parser.curr_byte++] = byte;
            bump_watchdog();

            // NMEA строки всегда заканчиваются переводом строки '\n' (0x0A)
            if(byte == 0x0A)
            {
                stop_watchdog();

                // Добавляем нуль-терминатор, чтобы буфер стал корректной C-строкой
                uart_parser.buffer[uart_parser.curr_byte] = '\0';

                // Проверяем, что это именно нужный нам пакет (например, RMC или GGA)
                // Ищем сигнатуру после '$' (индексы 1-5, например "GPRMC" или "GNRMC")
                if(strstr((char*)uart_parser.buffer, "RMC") != NULL)
                {
                    // Теперь пакет принят ПОЛНОСТЬЮ, можно безопасно парсить дату и время
                    parse_rmc_packet((char*)uart_parser.buffer);

                    // Переменные hour, minute, second должны обновляться внутри parse_rmc_packet!
                    // Убедитесь, что они объявлены как глобальные или передаются наружу.
                    display_send_data(3, DASH);
                    display_send_data(6, DASH);

                    display_send_data(2, numbers[second / 10]); // секунды
                    display_send_data(1, numbers[second % 10]);

                    display_send_data(5, numbers[minute / 10]); // минуты
                    display_send_data(4, numbers[minute % 10]);

                    display_send_data(8, numbers[hour / 10]);   // часы
                    display_send_data(7, numbers[hour % 10]);
                }

                reset_uart_parser();
            }
            break;
        }

        default:
            break;
    }
}

/* USER CODE END 1 */

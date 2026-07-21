/*
 * display.h
 *
 *  Created on: 20 апр. 2026 г.
 *      Author: georg
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <stdint.h>
#include "stm32f4xx.h"

#define CS_LOW GPIOA->BSRR = GPIO_BSRR_BR_4;
#define CS_HIGH GPIOA->BSRR = GPIO_BSRR_BS_4;

#define DASH 0b1

void display_send_data(uint16_t addr, uint16_t data);

#endif /* DISPLAY_H_ */

/*
 * display.h
 *
 *  Created on: 20 ���. 2026 �.
 *      Author: georg
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <stdint.h>
#include "stm32f4xx.h"


#define DASH 0b1

void display_send_data(uint16_t addr, uint16_t data);

#endif /* DISPLAY_H_ */

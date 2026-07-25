/*
 * time.h
 *
 *  Created on: Jul 16, 2026
 *      Author: katev
 */

#ifndef TIME_H
#define TIME_H

#include <stdint.h>

extern volatile uint32_t current_ntp_seconds;

void sync_time_from_gps(uint16_t year, uint8_t month, uint8_t day,uint8_t hour, uint8_t minute, uint8_t second);

uint32_t get_ntp_seconds(void);

uint32_t get_ntp_fraction(void);

#endif

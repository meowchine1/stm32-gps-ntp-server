/*
 * time.c
 *
 *  Created on: Jul 16, 2026
 *      Author: katev
 */


#include "time.h"
#include "stm32f4xx.h"


volatile uint32_t current_ntp_seconds = 0;
extern TIM_HandleTypeDef htim5;

/*
 * NTP epoch:
 * 01.01.1900 00:00:00
 *
 * Unix epoch:
 * 01.01.1970 00:00:00
 *
 * Difference:
 * 2208988800 seconds
 */


/*
 * Заглушка даты:
 *
 * 01.01.2026 00:00:00
 *
 * Unix timestamp:
 * 1767225600
 *
 * NTP timestamp:
 * 3976214400
 */
#define FIXED_DATE_NTP 3976214400UL


void sync_time_from_gps(uint8_t hour,
                        uint8_t minute,
                        uint8_t second)
{
    uint32_t seconds_today;

    seconds_today =
        hour * 3600UL +
        minute * 60UL +
        second;


    current_ntp_seconds =
        FIXED_DATE_NTP +
        seconds_today;


    /*
     * TIM5 работает как микросекундный счетчик
     */
    //TIM5->CNT = 0;
    __HAL_TIM_SET_COUNTER(&htim5, 0);
}



uint32_t get_ntp_seconds(void)
{
    return current_ntp_seconds;
}



uint32_t get_ntp_fraction(void)
{
    uint64_t us = TIM5->CNT;


    return (uint32_t)
        ((us * 4294967296ULL) / 1000000ULL);
}

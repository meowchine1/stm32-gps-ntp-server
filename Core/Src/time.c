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


//#define FIXED_DATE_NTP 3976214400UL


void sync_time_from_gps(uint16_t year,
                        uint8_t month,
                        uint8_t day,
                        uint8_t hour,
                        uint8_t minute,
                        uint8_t second)
{
    uint32_t days = 0;

    /*
     * Считаем количество дней от 01.01.1900 до нужной даты
     */
    for(uint16_t y = 1900; y < year; y++)
    {
        days += 365;

        if((y % 4 == 0 && y % 100 != 0) || (y % 400 == 0))
        {
            days++;
        }
    }

    /*
     * Количество дней в месяцах
     */
    uint8_t month_days[] =
    {
        31, 28, 31, 30,
        31, 30, 31, 31,
        30, 31, 30, 31
    };

    for(uint8_t m = 1; m < month; m++)
    {
        days += month_days[m - 1];

        // февраль в високосный год
        if(m == 2 &&
           ((year % 4 == 0 && year % 100 != 0) ||
            (year % 400 == 0)))
        {
            days++;
        }
    }

    /*
     * Добавляем дни текущего месяца
     */
    days += day - 1;

    /*
     * Получаем NTP timestamp:
     * секунды от 01.01.1900 00:00:00 UTC
     */
    current_ntp_seconds =
          days * 86400UL
        + hour * 3600UL
        + minute * 60UL
        + second;

    /*
     * TIM5 работает как микросекундный счетчик.
     * Сбрасываем дробную часть NTP времени.
     */
    __HAL_TIM_SET_COUNTER(&htim5, 0);
}


uint32_t get_ntp_seconds_(void)
{
    return current_ntp_seconds;
}

uint32_t get_ntp_seconds(void)
{
    uint32_t extra_seconds = TIM5->CNT / 1000000ULL;
    return current_ntp_seconds + extra_seconds;
}

uint32_t get_ntp_fraction(void)
{
    // Берем только микросекунды внутри текущей секунды
    uint64_t us = TIM5->CNT % 1000000ULL;

    // Перевод микросекунд в 32-битное дробное число NTP
    return (uint32_t)((us * 4294967296ULL) / 1000000ULL);
}

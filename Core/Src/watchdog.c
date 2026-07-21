/*
 * watchdog.c
 *
 *  Created on: 20 ���. 2026 �.
 *      Author: georg
 *
 *      katev: moving georg code to HAL
 */

#include "watchdog.h"
#include "stm32f4xx.h"


// move to HAL_TIM_PeriodElapsedCallback
//void TIM2_IRQHandler()
//{
//	if(TIM2->SR & TIM_SR_UIF)
//	{
//		TIM2->SR = ~TIM_SR_UIF;
//		reset_uart_parser();
//		stop_watchdog();
//	}
//}

extern TIM_HandleTypeDef htim2;

void start_watchdog(void)
{
	HAL_TIM_Base_Start_IT(&htim2);
//	TIM2->CNT = 0;
//	TIM2->CR1 |= TIM_CR1_CEN;
}

void stop_watchdog(void)
{

	HAL_TIM_Base_Stop_IT(&htim2);
	//	TIM2->CR1 &= ~TIM_CR1_CEN;
}

void bump_watchdog(void)
{

    HAL_TIM_Base_Stop_IT(&htim2);
	__HAL_TIM_SET_COUNTER(&htim2, 0);
	HAL_TIM_Base_Start_IT(&htim2);
//	TIM2->CR1 &= ~TIM_CR1_CEN;
//	TIM2->CNT = 0;
//	TIM2->CR1 |= TIM_CR1_CEN;
}

/*
 * FreeModbus Libary: BARE Port
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id$
 */

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

/* ----------------------- static functions ---------------------------------*/
static void prvvTIMERExpiredISR( void );

extern TIM_HandleTypeDef htim12;

/* ----------------------- Start implementation -----------------------------*/
// 初始化 TIM12
void MX_TIM12_Init(void) {
    __HAL_RCC_TIM12_CLK_ENABLE();
    htim12.Instance = TIM12;
    htim12.Init.Prescaler = (SystemCoreClock / 1000000) - 1; // 1us 计数
    htim12.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim12.Init.Period = 0xFFFF; // 初始最大周期
    htim12.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_Base_Init(&htim12);
}

// 初始化 Modbus 定时器
BOOL xMBPortTimersInit( USHORT usTim1Timerout50us )
{
    MX_TIM12_Init();
    htim12.Init.Period = usTim1Timerout50us * 50 - 1; // 50us 单位转换为微秒
    if(HAL_TIM_Base_Init(&htim12) != HAL_OK)
    {
        return FALSE;
    }
    
    // 配置中断优先级
    HAL_NVIC_SetPriority(TIM8_BRK_TIM12_IRQn, 6, 0);
    HAL_NVIC_EnableIRQ(TIM8_BRK_TIM12_IRQn);
    
    return TRUE;
}


void vMBPortTimersEnable(void)
{
    __HAL_TIM_SET_COUNTER(&htim12, 0);
    HAL_TIM_Base_Start_IT(&htim12);
}

// 禁用定时器
void vMBPortTimersDisable(void)
{
    HAL_TIM_Base_StopInit(&htim12);
}

/* Create an ISR which is called whenever the timer has expired. This function
 * must then call pxMBPortCBTimerExpired( ) to notify the protocol stack that
 * the timer has expired.
 */
void prvvTIMERExpiredISR( void )
{
    ( void )pxMBPortCBTimerExpired(  );
}


// TIM12 中断处理函数
void TIM8_BRK_TIM12_IRQHandler(void) {
    if (__HAL_TIM_GET_FLAG(&htim12, TIM_FLAG_UPDATE) != RESET) {
        __HAL_TIM_CLEAR_IT(&htim12, TIM_FLAG_UPDATE);
        vMBPortTimersDisable();
        pxMBPortCBTimerExpired(); // 通知协议栈超时
    }
}

#include "timer.h"
#include "stm32f4xx_hal.h"
#include "pid.h"

extern TIM_HandleTypeDef htim13;

void TIMER_Init(void) {
    htim13.Instance = TIM13;
    htim13.Init.Prescaler = 84 - 1;
    htim13.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim13.Init.Period = 5000 - 1; // 1ms中断
    HAL_TIM_Base_Init(&htim13);
    HAL_TIM_Base_Start_IT(&htim13);
    HAL_NVIC_SetPriority(TIM8_UP_TIM13_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(TIM8_UP_TIM13_IRQn);
}

// 中断服务函数（在stm32f4xx_it.c中实现）
void TIM8_UP_TIM13_IRQHandler(void) {
    if (__HAL_TIM_GET_FLAG(&htim13, TIM_FLAG_UPDATE)) {
        __HAL_TIM_CLEAR_FLAG(&htim13, TIM_FLAG_UPDATE);
        PID_Update();
    }
}

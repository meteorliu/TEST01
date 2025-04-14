#include "pwm.h"
#include "pid.h"

extern TIM_HandleTypeDef htim1;
static uint16_t MaxDuty=0;
uint8_t Boost_Enable=0;

void PWM_Init(void) {
    TIM_OC_InitTypeDef sConfigOC = {0};
    htim1.Instance = TIM1;
    htim1.Init.Prescaler = 0;
    htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim1.Init.Period = (168000000 / 300000) - 1; // 600kHz PWM
    htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_PWM_Init(&htim1);

    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 0;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
		sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;  // 停止时输出为低电平
    HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
		//PWM_Disable();
		
		MaxDuty = htim1.Init.Period*MAX_OUT_DUTY/100;
}

void PWM_SetDuty(uint16_t duty) {
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, duty);
}

//获取最大允许输出占空比
uint16_t PWM_GetMaxDuty(void) {
    return MaxDuty;
		//return htim1.Init.Period*pid.maxduty/100;
}

void PWM_Enable(void) {
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
		Boost_Enable=1;
}

void PWM_Disable(void) {
    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
		Boost_Enable=-0;
}

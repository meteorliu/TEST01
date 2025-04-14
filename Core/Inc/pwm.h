#ifndef __PWM_H
#define __PWM_H

#include "stm32f4xx_hal.h"

extern uint8_t Boost_Enable;

#define MAX_OUT_DUTY 25

void PWM_Init(void);
void PWM_SetDuty(uint16_t duty);
uint16_t PWM_GetMaxDuty(void);
void PWM_Enable(void);
void PWM_Disable(void);

#endif

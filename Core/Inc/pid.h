#ifndef __PID_H
#define __PID_H

#include <stdint.h>

typedef struct {
    float Kp, Ki, Kd;
    float target_voltage;
    float current_voltage;
    float error_prev;
    float error_prev2;
    float pwm_duty;
    uint16_t ready_counter;
    uint8_t is_ready;
		uint8_t maxduty;
} PID_Controller;

void PID_Init(float Kp, float Ki, float Kd);
void PID_Update(void);
void PID_SetTargetVoltage(float voltage);
uint8_t PID_IsReady(void);
uint16_t PID_GetCurrentDuty(void);

extern PID_Controller pid; // 外部可访问（调试用）

#endif

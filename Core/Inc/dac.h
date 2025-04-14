#ifndef _DAC_H_
#define _DAC_H_

#include "stm32f4xx_hal.h"
#include <math.h>
#include "common.h"

#define VPP_2930MV  2000
#define VPP_2560MV 1748
#define VPP_2000MV 1365
#define VPP_1280MV 874
#define VPP_640MV 437
#define VPP_320MV 218
#define VPP_1000MV 683

extern DAC_HandleTypeDef hdac;
extern TIM_HandleTypeDef htim7;
extern DMA_HandleTypeDef hdma_dac1;

// 正弦波表（一个周期 128 点，12-bit DAC，0-4095）
#define SINE_POINTS 128
extern uint32_t sine_wave[SINE_POINTS];

// DAC1 当前频率
extern volatile uint32_t Out_dac1_freq; // 默认 100 Hz

#define DAC_REF_VOLTAGE 3000

void Init_DAC(void);
void Stop_DAC1_Sine(void);
void Set_DAC2_Voltage(uint16_t voltage);
void Configure_DAC1_Sine(uint32_t freq);
void Generate_Sine_Wave(uint16_t SetVPP);
uint32_t Get_DAC1_Freq(void);
void Update_DAC1_OutFreq(uint32_t freq);
void Update_DAC2_Voltage(uint16_t voltage);
#endif 

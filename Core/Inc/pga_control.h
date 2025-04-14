#ifndef PGA_CONTROL_H
#define PGA_CONTROL_H

#include "stm32f4xx_hal.h"

// AD8231 增益枚举
typedef enum {
    GAIN_1   = 0x00, // 000
    GAIN_2   = 0x01, // 001
    GAIN_4   = 0x02, // 010
    GAIN_8   = 0x03, // 011
    GAIN_16  = 0x04, // 100
    GAIN_32  = 0x05, // 101
    GAIN_64  = 0x06, // 110
    GAIN_128 = 0x07  // 111
} PGA_Gain_t;

// Vpp 调整状态枚举
typedef enum {
    VPP_ADJUSTING       = 0, // 调整中
    VPP_ADJUSTED        = 1, // 调整到合适增益
    VPP_MAX_GAIN_SMALL  = 2, // 增益最大，幅度偏小
    VPP_MIN_GAIN_OVERFLOW = 3 // 增益最小，幅度溢出
} Vpp_Adjust_State_t;

// GPIO 定义
#define CS1_PIN  GPIO_PIN_15 // PC15, 电压通道片选
#define CS1_PORT GPIOC
#define CS2_PIN  GPIO_PIN_5  // PE5, 电流通道片选
#define CS2_PORT GPIOE
#define A2_PIN   GPIO_PIN_2  // PE2
#define A2_PORT  GPIOE
#define A1_PIN   GPIO_PIN_3  // PE3
#define A1_PORT  GPIOE
#define A0_PIN   GPIO_PIN_4  // PE4
#define A0_PORT  GPIOE

// 外部访问的 Vpp 调整状态
extern volatile Vpp_Adjust_State_t voltage_vpp_state;
extern volatile Vpp_Adjust_State_t current_vpp_state;

// 函数声明
void PGA_Init(void);
void Set_Voltage_Gain(PGA_Gain_t gain);
void Set_Current_Gain(PGA_Gain_t gain);
PGA_Gain_t Adjust_Gain(uint16_t* data, uint16_t size, uint32_t sample_rate, uint32_t signal_freq, uint8_t is_voltage_channel);

#endif
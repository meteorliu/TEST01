#ifndef IO_CONTROL_H
#define IO_CONTROL_H

#include "stm32f4xx_hal.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// LED 控制
#define LED1_PIN        GPIO_PIN_13  // PC13
#define LED1_PORT       GPIOC
#define LED2_PIN        GPIO_PIN_1  // PC1
#define LED2_PORT       GPIOC

// 电容滤波开关
#define CAP_SW1_PIN     GPIO_PIN_3  // PC3
#define CAP_SW1_PORT    GPIOC
#define CAP_SW2_PIN     GPIO_PIN_2  // PC2 (与 LED1 复用，需注意)
#define CAP_SW2_PORT    GPIOC
#define CAP_SW3_PIN     GPIO_PIN_6  // PE6
#define CAP_SW3_PORT    GPIOE

// PGA 休眠控制
#define PGA_SD_PIN      GPIO_PIN_14 // PC14
#define PGA_SD_PORT     GPIOC

// 继电器开关
#define RELAY1_LCR_IR_PIN  GPIO_PIN_0  // PE0
#define RELAY1_LCR_IR_PORT GPIOE
#define RELAY2_LCR_PIN     GPIO_PIN_1  // PE1
#define RELAY2_LCR_PORT    GPIOE
#define RELAY3_IR_PIN      GPIO_PIN_9  // PB9
#define RELAY3_IR_PORT     GPIOB

// 高压放电负载开关
#define SW_LOAD1_PIN    GPIO_PIN_8  // PB8
#define SW_LOAD1_PORT   GPIOB
#define SW_LOAD2_PIN    GPIO_PIN_6  // PB6
#define SW_LOAD2_PORT   GPIOB
#define SW_LOAD3_PIN    GPIO_PIN_7  // PB7
#define SW_LOAD3_PORT   GPIOB

// 定时器标志
extern volatile uint8_t flag_1ms;
extern volatile uint8_t flag_10ms;
extern volatile uint8_t flag_100ms;
extern volatile uint8_t flag_1s;

// 函数声明
void IO_Control_Init(void);
void TIM14_Init(void);
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);

// LED 控制函数
void LED1_Set_Flash(uint16_t period_ms, uint8_t count);
void LED2_Set_Breath(uint8_t enable, uint16_t period_ms);

// 电容滤波开关控制
void CAP_SW_Set(uint8_t sw1, uint8_t sw2, uint8_t sw3);

// PGA 休眠控制
void PGA_Set_Sleep(uint8_t sleep);

// 继电器开关控制
void Relay_Set(uint8_t lcr_ir, uint8_t lcr, uint8_t ir);

// 高压放电负载开关控制
void Load_Switch_Set(uint8_t load1, uint8_t load2, uint8_t load3);

void BaseTimerProcess(void);
void HAL_TIM14_Callback(TIM_HandleTypeDef *htim);
	
#endif

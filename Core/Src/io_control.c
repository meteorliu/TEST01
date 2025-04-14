#include "io_control.h"
#include "pid.h"
#include "pwm.h"
#include <math.h>

// 定时器计数器和标志
static volatile uint16_t timer_counter = 0;
volatile uint8_t flag_1ms = 0;
volatile uint8_t flag_10ms = 0;
volatile uint8_t flag_100ms = 0;
volatile uint8_t flag_1s = 0;

// LED1 控制参数
static uint16_t led1_period = 0;
static uint8_t led1_count = 0;
static uint16_t led1_tick = 0;
static uint8_t led1_state = 0;

// LED2 呼吸灯参数
static uint8_t led2_enable = 0;
static uint16_t led2_period = 0;
static uint16_t led2_tick = 0;

// 初始化 IO 和定时器
void IO_Control_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // 启用时钟
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    // 配置所有引脚为输出
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

    // LED 引脚
    GPIO_InitStruct.Pin = LED1_PIN | LED2_PIN;
    HAL_GPIO_Init(LED1_PORT, &GPIO_InitStruct);

    // 电容滤波开关
    GPIO_InitStruct.Pin = CAP_SW1_PIN;
    HAL_GPIO_Init(CAP_SW1_PORT, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = CAP_SW3_PIN;
    HAL_GPIO_Init(CAP_SW3_PORT, &GPIO_InitStruct);

    // PGA 休眠
    GPIO_InitStruct.Pin = PGA_SD_PIN;
    HAL_GPIO_Init(PGA_SD_PORT, &GPIO_InitStruct);

    // 继电器
    GPIO_InitStruct.Pin = RELAY1_LCR_IR_PIN | RELAY2_LCR_PIN;
    HAL_GPIO_Init(RELAY1_LCR_IR_PORT, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = RELAY3_IR_PIN;
    HAL_GPIO_Init(RELAY3_IR_PORT, &GPIO_InitStruct);

    // 高压放电负载开关
    GPIO_InitStruct.Pin = SW_LOAD1_PIN | SW_LOAD2_PIN | SW_LOAD3_PIN;
    HAL_GPIO_Init(SW_LOAD1_PORT, &GPIO_InitStruct);

    // 初始化状态
    HAL_GPIO_WritePin(LED1_PORT, LED1_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LED2_PORT, LED2_PIN, GPIO_PIN_RESET);
    CAP_SW_Set(0, 0, 0);
    PGA_Set_Sleep(0);
    Relay_Set(0, 0, 0);
    Load_Switch_Set(0, 0, 0);
		LED1_Set_Flash(500,0);

    TIM14_Init();
}

// 定时器 14 初始化 (1ms 中断)
void TIM14_Init(void) {
    TIM_HandleTypeDef htim14;
    __HAL_RCC_TIM14_CLK_ENABLE();

    htim14.Instance = TIM14;
    htim14.Init.Prescaler = (SystemCoreClock / 1000000) - 1; // 1us 时基
    htim14.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim14.Init.Period = 999; // 1ms 中断
    htim14.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_Base_Init(&htim14);
	
		// 确保非单次模式
    TIM14->CR1 &= ~TIM_CR1_OPM;
    TIM14->SMCR = 0; // 禁用从模式


    //HAL_NVIC_SetPriority(TIM8_TRG_COM_TIM14_IRQn, 3, 3);
    //HAL_NVIC_EnableIRQ(TIM8_TRG_COM_TIM14_IRQn);
    HAL_TIM_Base_Start_IT(&htim14);
}

void TIM14_IRQHandler(void) {
    if (TIM14->SR & TIM_SR_UIF) {
        TIM14->SR &= ~TIM_SR_UIF;
        // 添加功能代码，避免清零 CEN
    }
}

//非实时要求的定时需要处理的程序在这里运行
void BaseTimerProcess(void){
	static uint16_t led2_pwm_counter=0;
	if(flag_1ms)
	{
		flag_1ms = 0;
		// LED1 闪烁控制
        if (led1_period > 0) {
            led1_tick++;
            if (led1_tick >= led1_period) {
                led1_tick = 0;
                led1_state = !led1_state;
                HAL_GPIO_WritePin(LED1_PORT, LED1_PIN, led1_state ? GPIO_PIN_SET : GPIO_PIN_RESET);
                if (led1_count > 0 && !led1_state) {
                    led1_count--;
                    if (led1_count == 0) {
                        led1_period = 0; // 停止闪烁
                        HAL_GPIO_WritePin(LED1_PORT, LED1_PIN, GPIO_PIN_RESET);
                    }
                }
            }
        }

        // LED2 呼吸灯控制（软件 PWM）
        if (led2_enable && led2_period > 0 && Boost_Enable) {
            led2_tick++;
            if (led2_tick >= led2_period) led2_tick = 0;

            // 计算当前相位和亮度
            float phase = (2.0f * (float)M_PI * led2_tick) / led2_period;
            float brightness = (sinf(phase) + 1.0f) / 2.0f; // 0 到 1

            // 软件 PWM：在一个 10ms 周期内控制高低电平时间
            led2_pwm_counter++;
            if (led2_pwm_counter >= 10) led2_pwm_counter = 0; // 10ms 周期
            uint16_t high_time = (uint16_t)(brightness * 10); // 高电平时间（ms）
            if (led2_pwm_counter < high_time) {
                HAL_GPIO_WritePin(LED2_PORT, LED2_PIN, GPIO_PIN_SET);
            } else {
                HAL_GPIO_WritePin(LED2_PORT, LED2_PIN, GPIO_PIN_RESET);
            }
        }
	}
	
	if(flag_1s)
	{
		static uint16_t temp=0;
		flag_1s = 0;
		if(Boost_Enable)
		{
			//提升电压是否到达设定电压值
			if(PID_IsReady()){
				LED2_Set_Breath(1,0);
			}
			else
			{
				LED2_Set_Breath(1,200);
			}
		}
		temp++;
//		if(temp&0x01)
//			Relay_Set(0,0,0);
//		else
//			Relay_Set(1,1,1);
	}
	
	if(flag_10ms)
	{
		flag_10ms = 0;
		//PID_Update();
	}
}
// 定时器中断回调
void HAL_TIM14_Callback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM14) {
        timer_counter++;
        flag_1ms = 1;
        if (timer_counter % 10 == 0) flag_10ms = 1;
        if (timer_counter % 100 == 0) flag_100ms = 1;
        if (timer_counter % 1000 == 0) {
            flag_1s = 1;
            timer_counter = 0; // 复位计数器
        }
    }
}

// LED1 设置闪烁
void LED1_Set_Flash(uint16_t period_ms, uint8_t count) {
    led1_period = period_ms;
    led1_count = count;
    led1_tick = 0;
    led1_state = 0;
    if (period_ms == 0) {
        HAL_GPIO_WritePin(LED1_PORT, LED1_PIN, count? GPIO_PIN_SET:GPIO_PIN_RESET);
    }
}

// LED2 设置呼吸灯
void LED2_Set_Breath(uint8_t enable, uint16_t period_ms) {
    led2_enable = enable;
    led2_period = period_ms;
    led2_tick = 0;
    if (!enable) {
        HAL_GPIO_WritePin(LED2_PORT, LED2_PIN, GPIO_PIN_RESET);
    }else{
			if(period_ms ==0)
			{
				HAL_GPIO_WritePin(LED2_PORT, LED2_PIN, GPIO_PIN_SET);
			}
		}
}

// 电容滤波开关控制
void CAP_SW_Set(uint8_t sw1, uint8_t sw2, uint8_t sw3) {
    HAL_GPIO_WritePin(CAP_SW1_PORT, CAP_SW1_PIN, sw1 ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(CAP_SW2_PORT, CAP_SW2_PIN, sw2 ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(CAP_SW3_PORT, CAP_SW3_PIN, sw3 ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

// PGA 休眠控制
void PGA_Set_Sleep(uint8_t sleep) {
    HAL_GPIO_WritePin(PGA_SD_PORT, PGA_SD_PIN, sleep ? GPIO_PIN_RESET : GPIO_PIN_SET);
}

// 继电器开关控制
void Relay_Set(uint8_t lcr_ir, uint8_t lcr, uint8_t ir) {
    HAL_GPIO_WritePin(RELAY1_LCR_IR_PORT, RELAY1_LCR_IR_PIN, lcr_ir ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(RELAY2_LCR_PORT, RELAY2_LCR_PIN, lcr ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(RELAY3_IR_PORT, RELAY3_IR_PIN, ir ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

// 高压放电负载开关控制
void Load_Switch_Set(uint8_t load1, uint8_t load2, uint8_t load3) {
    HAL_GPIO_WritePin(SW_LOAD1_PORT, SW_LOAD1_PIN, load1 ? GPIO_PIN_SET : GPIO_PIN_RESET);
    // SW_LOAD2 和 SW_LOAD3 需同时控制
    if (load2 && load3) {
        HAL_GPIO_WritePin(SW_LOAD2_PORT, SW_LOAD2_PIN, GPIO_PIN_SET);
        HAL_GPIO_WritePin(SW_LOAD3_PORT, SW_LOAD3_PIN, GPIO_PIN_SET);
    } else {
        HAL_GPIO_WritePin(SW_LOAD2_PORT, SW_LOAD2_PIN, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(SW_LOAD3_PORT, SW_LOAD3_PIN, GPIO_PIN_RESET);
    }
}

#ifndef _LCR_ADC_H_
#define _LCR_ADC_H_
#include "stm32f4xx_hal.h"

// ADC 和 DMA 句柄（假设由 CubeMX 生成）
extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;
extern ADC_HandleTypeDef hadc3; // 新增 ADC3
extern DMA_HandleTypeDef hdma_adc1;
extern DMA_HandleTypeDef hdma_adc2;
extern DMA_HandleTypeDef hdma_adc3; // 新增 ADC3 DMA
extern TIM_HandleTypeDef htim2; // 改为 TIM2

// 定义缓冲区
#define SAMPLE_SIZE 512
#define MODE4_SIZE 10
extern uint16_t adc2_buffer_mode123[SAMPLE_SIZE]; // MODE 1,2,3: ADC2 数据
extern uint16_t adc3_buffer_mode123[SAMPLE_SIZE]; // MODE 1,2,3: ADC3 数据
extern uint16_t adc_buffer_mode4[MODE4_SIZE * 2]; // Mode 4: 10个 IN8 + 10个 IN9
extern uint16_t adc1_data[SAMPLE_SIZE];           // 分离后的 ADC1 数据（保留，当前未用）
extern uint16_t adc2_data[SAMPLE_SIZE];           // 分离后的 ADC2 数据
extern uint16_t adc3_data[SAMPLE_SIZE];           // 分离后的 ADC3 数据

// 模式枚举
typedef enum {
    MODE_1 = 0,
    MODE_2,
    MODE_3,
    MODE_4
} ADC_Mode_TypeDef;

// 采样时间枚举
typedef enum {
    SAMPLE_TIME_3CYCLES   = ADC_SAMPLETIME_3CYCLES,
    SAMPLE_TIME_15CYCLES  = ADC_SAMPLETIME_15CYCLES,
    SAMPLE_TIME_28CYCLES  = ADC_SAMPLETIME_28CYCLES,
    SAMPLE_TIME_56CYCLES  = ADC_SAMPLETIME_56CYCLES,
    SAMPLE_TIME_84CYCLES  = ADC_SAMPLETIME_84CYCLES,
    SAMPLE_TIME_112CYCLES = ADC_SAMPLETIME_112CYCLES,
    SAMPLE_TIME_144CYCLES = ADC_SAMPLETIME_144CYCLES,
    SAMPLE_TIME_480CYCLES = ADC_SAMPLETIME_480CYCLES
} SampleTime_t;

// 模式配置结构体
typedef struct {
    ADC_Mode_TypeDef mode;      // 模式编号
    uint32_t adc2_channel;      // ADC2 通道（原 adc1_channel）
    uint32_t adc3_channel;      // ADC3 通道（原 adc2_channel）
    uint16_t sample_size;       // 单次采样数量
    uint8_t is_dual_mode;       // 是否为原 Dual Mode（现仅标志 MODE_4）
    SampleTime_t sample_time;   // 采样时间
} ADC_Config_t;


// 当前模式和采样率
extern volatile  ADC_Mode_TypeDef current_mode;
//volatile uint32_t sample_rate = 12800; // 默认 12.8 kHz
extern volatile uint8_t adc_conversion_complete; // 转换完成标志

void Switch_ADC_Mode(ADC_Mode_TypeDef mode, uint32_t freq);

#endif

#include "boost_adc.h"
#include "stm32f4xx_hal.h"

#define ADC_REF_VOLT  3.0f  //MCU参考电压
#define VOLTAGE_RATIO 51.0f //取样电路缩小倍数

extern ADC_HandleTypeDef hadc1;
float SET_Voltage;

float Boost_ADC_ReadVoltage(void) {
    HAL_ADC_Start(&hadc1);
    if (HAL_ADC_PollForConversion(&hadc1, 1) == HAL_OK) {
        uint16_t adc_val = HAL_ADC_GetValue(&hadc1);
				SET_Voltage = (adc_val * ADC_REF_VOLT / 4095.0f) * VOLTAGE_RATIO;
				
        return SET_Voltage;
    }
    return 0.0f;
}

// ADC初始化（在main.c中调用）
void Boost_ADC_Init(void) {
    ADC_ChannelConfTypeDef sConfig = {0};
    hadc1.Instance = ADC1;
    hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
    hadc1.Init.Resolution = ADC_RESOLUTION_12B;
    hadc1.Init.ScanConvMode = DISABLE;
    hadc1.Init.ContinuousConvMode = DISABLE;
    HAL_ADC_Init(&hadc1);

    sConfig.Channel = ADC_CHANNEL_9;
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_28CYCLES;
    HAL_ADC_ConfigChannel(&hadc1, &sConfig);
}

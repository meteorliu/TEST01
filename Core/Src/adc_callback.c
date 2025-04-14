#include "stm32f4xx_hal.h"
#include "lcr_adc.h"
#include "dac.h"
#if 0
// DMA 传输完成中断 (Mode 1, 2, 3 使用 ADC1 的 DMA 中断)
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
    if (hadc->Instance == ADC1 && current_mode != MODE_4) {
        // Mode 1, 2, 3 数据采集完成
        HAL_ADCEx_MultiModeStop(&hadc1); // 停止 ADC
        HAL_TIM_Base_Stop(&htim6);
        // 数据分离在主循环中处理
        adc_conversion_complete = 1;      // 设置完成标志
    }
    else if (hadc->Instance == ADC2 && current_mode == MODE_4) {
        // Mode 4 数据采集完成
        HAL_ADC_Stop(&hadc2);
        adc_conversion_complete = 1;      // 设置完成标志
    }
}
#endif

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
    if ((hadc->Instance == ADC2 || hadc->Instance == ADC3) && current_mode != MODE_4) {
        
        HAL_TIM_Base_Stop(&htim2);
        adc_conversion_complete = 1; // 设置完成标志
    }
    else if (hadc->Instance == ADC2 && current_mode == MODE_4) {
        // Mode 4 数据采集完成
        HAL_ADC_Stop(&hadc2);
        adc_conversion_complete = 1;      // 设置完成标志
    }
}

void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc) {
    if (current_mode != MODE_4) {
        // 处理前半部分数据
        //Separate_ADC_Data(0, SAMPLE_SIZE / 2);
    }
}

void HAL_ADC_ErrorCallback(ADC_HandleTypeDef* hadc) {
    if (hadc->ErrorCode != HAL_ADC_ERROR_NONE) {
        // 处理错误，例如重启 ADC
        HAL_ADC_Stop(hadc);
        Switch_ADC_Mode(current_mode, Get_DAC1_Freq()); // 恢复当前模式
    }
}

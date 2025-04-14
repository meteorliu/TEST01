#include "lcr_adc.h"
#include "adc_process.h"

void Separate_ADC_Data(void){
    if (current_mode == MODE_1 || current_mode == MODE_2 || current_mode == MODE_3) {
        // Mode 1, 2, 3: 交错数据分离
        for (uint16_t i = 0; i < SAMPLE_SIZE; i++) {
            adc2_data[i] = adc2_buffer_mode123[2 * i];     // ADC1 数据
            adc3_data[i] = adc3_buffer_mode123[2 * i + 1]; // ADC2 数据
        }
    }
    else if (current_mode == MODE_4) {
        // Mode 4: IN8 和 IN9 分离
        for (uint16_t i = 0; i < MODE4_SIZE; i++) {
            adc2_data[i] = adc_buffer_mode4[i];           // IN8 (假设存储在 adc1_data)
            adc3_data[i] = adc_buffer_mode4[i + MODE4_SIZE]; // IN9
        }
    }
}

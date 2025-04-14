#include "lcr_adc.h"

// 模式配置表
static const ADC_Config_t adc_configs[] = {
    {MODE_1, ADC_CHANNEL_2, ADC_CHANNEL_3, SAMPLE_SIZE, 0, SAMPLE_TIME_15CYCLES}, // PA2 (ADC2), PA3 (ADC3)
    {MODE_2, ADC_CHANNEL_7, ADC_CHANNEL_4, SAMPLE_SIZE, 0, SAMPLE_TIME_15CYCLES}, // PA7 (ADC2), PA4 (ADC3)
    {MODE_3, ADC_CHANNEL_7, ADC_CHANNEL_5, SAMPLE_SIZE, 0, SAMPLE_TIME_15CYCLES}, // PA7 (ADC2), PA5 (ADC3)
    {MODE_4, ADC_CHANNEL_8, ADC_CHANNEL_9, MODE4_SIZE,  1, SAMPLE_TIME_28CYCLES}  // PB0, PB1 (ADC2)
};

// 当前模式和采样率
volatile ADC_Mode_TypeDef current_mode = MODE_1;
volatile uint32_t sample_rate = 12800; // 默认 12.8 kHz
volatile uint8_t adc_conversion_complete = 0; // 转换完成标志

uint16_t adc2_buffer_mode123[SAMPLE_SIZE]; // MODE 1,2,3: ADC2 数据
uint16_t adc3_buffer_mode123[SAMPLE_SIZE]; // MODE 1,2,3: ADC3 数据
uint16_t adc_buffer_mode4[MODE4_SIZE * 2]; // Mode 4: 10个 IN8 + 10个 IN9
uint16_t adc1_data[SAMPLE_SIZE];           // 分离后的 ADC1 数据（保留，当前未用）
uint16_t adc2_data[SAMPLE_SIZE];           // 分离后的 ADC2 数据
uint16_t adc3_data[SAMPLE_SIZE];           // 分离后的 ADC3 数据

// 配置定时器 2 的触发频率
void Configure_TIM2(uint32_t freq) {
    uint32_t tim_clk = 84000000; // 假设 TIM2 时钟为 84 MHz
    uint16_t prescaler = (tim_clk / freq) / 65536; // 处理溢出
    uint16_t period = (tim_clk / (prescaler + 1)) / freq - 1;

    __HAL_TIM_SET_PRESCALER(&htim2, prescaler);
    __HAL_TIM_SET_AUTORELOAD(&htim2, period);
    HAL_TIM_Base_Start(&htim2);
}

// 配置 ADC2 和 ADC3 独立采样模式 (Mode 1, 2, 3)
void Configure_Independent_Mode(const ADC_Config_t *config) {
    ADC_ChannelConfTypeDef sConfig = {0};

    // 停止之前的 ADC 和 DMA
    HAL_ADC_Stop(&hadc2);
    HAL_DMA_Abort(&hdma_adc2);
    HAL_ADC_Stop(&hadc3);
    HAL_DMA_Abort(&hdma_adc3);

    // 配置 ADC2
    hadc2.Instance->SQR1 &= ~(ADC_SQR1_L); // 单通道 (L = 0)
    sConfig.Channel = config->adc2_channel;
    sConfig.Rank = 1;
    sConfig.SamplingTime = config->sample_time;
    HAL_ADC_ConfigChannel(&hadc2, &sConfig);
    HAL_ADC_Start_DMA(&hadc2, (uint32_t*)adc2_buffer_mode123, config->sample_size);

    // 配置 ADC3
    hadc3.Instance->SQR1 &= ~(ADC_SQR1_L); // 单通道 (L = 0)
    sConfig.Channel = config->adc3_channel;
    sConfig.Rank = 1;
    sConfig.SamplingTime = config->sample_time;
    HAL_ADC_ConfigChannel(&hadc3, &sConfig);
    HAL_ADC_Start_DMA(&hadc3, (uint32_t*)adc3_buffer_mode123, config->sample_size);
}

// 配置 Mode 4: ADC2 规则转换模式 (IN8, IN9)
void Configure_Mode4(const ADC_Config_t *config) {
    ADC_ChannelConfTypeDef sConfig = {0};

    // 停止之前的 ADC 和 DMA
    HAL_ADC_Stop(&hadc2);
    HAL_DMA_Abort(&hdma_adc2);
    HAL_ADC_Stop(&hadc3);
    HAL_DMA_Abort(&hdma_adc3);

    // 配置 ADC2 为独立规则模式，2 个通道
    hadc2.Instance->SQR1 &= ~(ADC_SQR1_L);
    hadc2.Instance->SQR1 |= (1 << 20); // 设置 2 个通道 (L = 1)

    sConfig.SamplingTime = config->sample_time;
    // IN8 (PB0)
    sConfig.Channel = config->adc2_channel; // Mode 4 使用 adc2_channel 表示 IN8
    sConfig.Rank = 1;
    HAL_ADC_ConfigChannel(&hadc2, &sConfig);
    // IN9 (PB1)
    sConfig.Channel = config->adc3_channel; // Mode 4 使用 adc3_channel 表示 IN9
    sConfig.Rank = 2;
    HAL_ADC_ConfigChannel(&hadc2, &sConfig);

    // 启动 DMA
    HAL_ADC_Start_DMA(&hadc2, (uint32_t*)adc_buffer_mode4, config->sample_size * 2);
}

// 模式切换主函数
void Switch_ADC_Mode(ADC_Mode_TypeDef mode, uint32_t freq) {
    const ADC_Config_t* config = &adc_configs[mode];

    // 检查 ADC 和 DMA 是否空闲
    while (HAL_ADC_GetState(&hadc2) == HAL_ADC_STATE_BUSY || 
           HAL_ADC_GetState(&hadc3) == HAL_ADC_STATE_BUSY || 
           HAL_DMA_GetState(&hdma_adc2) == HAL_DMA_STATE_BUSY || 
           HAL_DMA_GetState(&hdma_adc3) == HAL_DMA_STATE_BUSY) {
        HAL_Delay(1);
    }

    // 更新全局状态
    current_mode = mode;
    if (config->is_dual_mode) { // MODE_4
        sample_rate = freq;
    } else { // MODE_1,2,3
        if (freq == 100) sample_rate = 12800;
        else if (freq == 1000) sample_rate = 128000;
        else if (freq == 10000) sample_rate = 1280000;
    }
    adc_conversion_complete = 0;

    // 配置定时器
    Configure_TIM2(sample_rate); // 使用实际采样率

    // 根据模式配置 ADC
    if (config->is_dual_mode) {
        Configure_Mode4(config);
    } else {
        Configure_Independent_Mode(config);
    }
}

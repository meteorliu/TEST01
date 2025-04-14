#include "dac.h"

uint32_t sine_wave[SINE_POINTS];

// DAC1 当前频率
volatile uint32_t Out_dac1_freq = 1000; // 默认 100 Hz

void Init_DAC(void){
    Generate_Sine_Wave(VPP_2560MV);
		Configure_DAC1_Sine(Out_dac1_freq);
		Set_DAC2_Voltage(0);
}


// 生成正弦波表（在初始化时调用）
void Generate_Sine_Wave(uint16_t SetVPP) {
    for (uint16_t i = 0; i < SINE_POINTS; i++) {
        sine_wave[i] = (uint16_t)(90 + SetVPP * (1.0 + sin(2.0 * M_PI * i / SINE_POINTS))); // 0-4095
    }
}

// 配置 TIM7 触发频率
void Configure_TIM7(uint32_t dac_freq) {
    uint32_t tim_clk = 84000000; // 假设 TIM7 时钟为 84 MHz (APB1)
    uint32_t sample_rate = dac_freq * SINE_POINTS; // 一个周期 128 点
    uint16_t prescaler = (tim_clk / sample_rate) / 65536; // 处理溢出
    uint16_t period = (tim_clk / (prescaler + 1)) / sample_rate - 1;

    __HAL_TIM_SET_PRESCALER(&htim7, prescaler);
    __HAL_TIM_SET_AUTORELOAD(&htim7, period);
    HAL_TIM_Base_Start(&htim7);
}

// 配置 DAC1 输出正弦波
void Configure_DAC1_Sine(uint32_t freq) {
    DAC_ChannelConfTypeDef sConfig = {0};

    // 停止之前的 DAC 和 DMA
    HAL_DAC_Stop_DMA(&hdac, DAC_CHANNEL_1);
    HAL_DMA_Abort(&hdma_dac1);

    // 更新频率
    Out_dac1_freq = freq;

    // 配置 TIM7 触发频率
    Configure_TIM7(freq);

    // 配置 DAC1 通道 1
    sConfig.DAC_Trigger = DAC_TRIGGER_T7_TRGO; // TIM7 触发
    sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
    HAL_DAC_ConfigChannel(&hdac, &sConfig, DAC_CHANNEL_1);

    // 启动 DMA 循环模式
    HAL_DAC_Start_DMA(&hdac, DAC_CHANNEL_1, (uint32_t*)sine_wave, SINE_POINTS, DAC_ALIGN_12B_R);
}

// 设置 DAC2 输出电压 单位MV
void Set_DAC2_Voltage(uint16_t voltage) {

    // Vref = 3.0V，12-bit DAC，电压范围 0-3.0V
    if (voltage > DAC_REF_VOLTAGE) voltage = DAC_REF_VOLTAGE;

    uint16_t dac_value = (uint16_t)((voltage / DAC_REF_VOLTAGE) * 4095); // 转换为 12-bit 值

    DAC_ChannelConfTypeDef sConfig = {0};
    sConfig.DAC_Trigger = DAC_TRIGGER_NONE; // 无触发，直接输出
    sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
    HAL_DAC_ConfigChannel(&hdac, &sConfig, DAC_CHANNEL_2);

    // 设置并启动 DAC2
    HAL_DAC_SetValue(&hdac, DAC_CHANNEL_2, DAC_ALIGN_12B_R, dac_value);
    HAL_DAC_Start(&hdac, DAC_CHANNEL_2);
}

//更新DAC2的输出电压，控制喷油器工作电流
void Update_DAC2_Voltage(uint16_t voltage) {
		// Vref = 3.0V，12-bit DAC，电压范围 0-3.0V
    if (voltage > DAC_REF_VOLTAGE) voltage = DAC_REF_VOLTAGE;

    uint16_t dac_value = (uint16_t)((voltage / DAC_REF_VOLTAGE) * 4095); // 转换为 12-bit 值
		// 设置并启动 DAC2
    HAL_DAC_SetValue(&hdac, DAC_CHANNEL_2, DAC_ALIGN_12B_R, dac_value);
    //HAL_DAC_Start(&hdac, DAC_CHANNEL_2);
}

//更新DAC1的输出频率
void Update_DAC1_OutFreq(uint32_t freq) {
		// 更新频率
    Out_dac1_freq = freq;
		// 配置 TIM7 触发频率
    Configure_TIM7(freq);
}

//获取DAC1的输出频率
uint32_t Get_DAC1_Freq(void)
{
		return Out_dac1_freq;
}

// 停止 DAC1 正弦波输出
void Stop_DAC1_Sine(void) {
    // 停止 DMA 传输
    HAL_DAC_Stop_DMA(&hdac, DAC_CHANNEL_1);
    
    // 停止 TIM7 触发
    HAL_TIM_Base_Stop(&htim7);
    
    // 停止 DAC1 输出
    HAL_DAC_Stop(&hdac, DAC_CHANNEL_1);
    
    // 可选：将 DAC 输出设置为 0V（避免悬浮）
    HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, 0);
}


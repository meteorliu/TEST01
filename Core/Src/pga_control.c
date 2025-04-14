#include "pga_control.h"
#include <math.h>

// 全局变量保存当前增益
static PGA_Gain_t voltage_gain = GAIN_1; // 电压通道当前增益
static PGA_Gain_t current_gain = GAIN_1; // 电流通道当前增益

// Vpp 调整状态
volatile Vpp_Adjust_State_t voltage_vpp_state = VPP_ADJUSTING;
volatile Vpp_Adjust_State_t current_vpp_state = VPP_ADJUSTING;

static void Set_PGA_Gain(PGA_Gain_t gain, GPIO_TypeDef* cs_port, uint16_t cs_pin) {
    HAL_GPIO_WritePin(cs_port, cs_pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(A2_PORT, A2_PIN, (gain & 0x04) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(A1_PORT, A1_PIN, (gain & 0x02) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(A0_PORT, A0_PIN, (gain & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(cs_port, cs_pin, GPIO_PIN_RESET);
}

void PGA_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    GPIO_InitStruct.Pin = CS1_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(CS1_PORT, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = CS2_PIN | A2_PIN | A1_PIN | A0_PIN;
    HAL_GPIO_Init(CS2_PORT, &GPIO_InitStruct);

    // 初始化增益并更新全局变量
    voltage_gain = GAIN_1;
    current_gain = GAIN_1;
    voltage_vpp_state = VPP_ADJUSTING; // 初始状态为调整中
    current_vpp_state = VPP_ADJUSTING;
    Set_Voltage_Gain(GAIN_1);
    Set_Current_Gain(GAIN_1);
}

void Set_Voltage_Gain(PGA_Gain_t gain) {
    Set_PGA_Gain(gain, CS1_PORT, CS1_PIN);
}

void Set_Current_Gain(PGA_Gain_t gain) {
    Set_PGA_Gain(gain, CS2_PORT, CS2_PIN);
}

PGA_Gain_t Adjust_Gain(uint16_t* data, uint16_t size, uint32_t sample_rate, uint32_t signal_freq, uint8_t is_voltage_channel) {
    // 计算峰峰值
    uint16_t max_val = data[0];
    uint16_t min_val = data[0];
    uint8_t overflow = 0;
    for (uint16_t i = 1; i < size; i++) {
        if (data[i] > max_val) max_val = data[i];
        if (data[i] < min_val) min_val = data[i];
        if (data[i] >= 4000 || data[i] <= 100) overflow = 1; // 接近 3V 或 0V
    }
    float vpp = (max_val - min_val) * 3.0f / 4095.0f;

    // 使用全局变量获取当前增益
    PGA_Gain_t lasttime_gain = is_voltage_channel ? voltage_gain : current_gain;
    float gain_value = 1.0f;
    switch (lasttime_gain) {
        case GAIN_1: gain_value = 1; break;
        case GAIN_2: gain_value = 2; break;
        case GAIN_4: gain_value = 4; break;
        case GAIN_8: gain_value = 8; break;
        case GAIN_16: gain_value = 16; break;
        case GAIN_32: gain_value = 32; break;
        case GAIN_64: gain_value = 64; break;
        case GAIN_128: gain_value = 128; break;
    }

    // 计算实际峰峰值
    float real_vpp;
    if (overflow) {
        // 溢出时，使用零点斜率预估
        int16_t zero_cross_idx = -1;
        for (uint16_t i = 0; i < size - 1; i++) {
            if ((data[i] <= 2048 && data[i + 1] >= 2048) || (data[i] >= 2048 && data[i + 1] <= 2048)) {
                zero_cross_idx = i;
                break;
            }
        }

        if (zero_cross_idx != -1) {
            float delta_v = (data[zero_cross_idx + 1] - data[zero_cross_idx]) * 3.0f / 4095.0f;
            float dt = 1.0f / sample_rate;
            float actual_slope = delta_v / dt;
            float angle_step = 2.0f * M_PI / SINE_POINTS; // 128 点正弦波
            float std_slope = sinf(angle_step) / dt; // 单位幅度斜率
            float peak_amplitude = fabsf(actual_slope / std_slope); // 峰值
            real_vpp = 2.0f * peak_amplitude * gain_value; // 峰峰值
        } else {
            real_vpp = 3.0f * gain_value; // 默认最大峰峰值
        }
    } else {
        real_vpp = vpp * gain_value; // 未溢出时直接使用测量值
    }

    // 目标范围 1.45Vpp - 2.9Vpp，参考值取 2.0Vpp
    PGA_Gain_t new_gain = current_gain;
    float gain_adjustment;

    // 计算增益调整步长并避免溢出
    if (real_vpp > 2.9f || real_vpp < 1.45f) {
        gain_adjustment = log2f(2.0f / real_vpp); // 调整步长
        float new_gain_float = current_gain + gain_adjustment;

        // 提前限制范围，避免溢出
        if (new_gain_float >= 7.0f) {
            new_gain = GAIN_128;
        } else if (new_gain_float <= 0.0f) {
            new_gain = GAIN_1;
        } else {
            new_gain = (PGA_Gain_t)(new_gain_float + 0.5f); // 四舍五入
        }
    }

    // 更新 Vpp 调整状态
    Vpp_Adjust_State_t* state = is_voltage_channel ? &voltage_vpp_state : &current_vpp_state;
    if (real_vpp >= 1.45f && real_vpp <= 2.9f) {
        *state = VPP_ADJUSTED; // 调整到合适增益
    } else if (new_gain == GAIN_128 && real_vpp < 1.45f) {
        *state = VPP_MAX_GAIN_SMALL; // 增益最大，幅度偏小
    } else if (new_gain == GAIN_1 && real_vpp > 2.9f) {
        *state = VPP_MIN_GAIN_OVERFLOW; // 增益最小，幅度溢出
    } else {
        *state = VPP_ADJUSTING; // 调整中
    }

    return new_gain;
}

#if 0
//有效值方式

PGA_Gain_t Adjust_Gain(uint16_t* data, uint16_t size, uint32_t sample_rate, uint32_t signal_freq, uint8_t is_voltage_channel) {
    // 计算 RMS 幅值
    float sum = 0;
    uint8_t overflow = 0;
    for (uint16_t i = 0; i < size; i++) {
        float voltage = (data[i] * 3.0f) / 4095.0f;
        sum += voltage * voltage;
        if (data[i] >= 4000 || data[i] <= 100) overflow = 1;
    }
    float amplitude = sqrtf(sum / size);

    PGA_Gain_t current_gain = is_voltage_channel ? voltage_gain : current_gain;
    float gain_value = 1.0f;
    switch (current_gain) {
        case GAIN_1: gain_value = 1; break;
        case GAIN_2: gain_value = 2; break;
        case GAIN_4: gain_value = 4; break;
        case GAIN_8: gain_value = 8; break;
        case GAIN_16: gain_value = 16; break;
        case GAIN_32: gain_value = 32; break;
        case GAIN_64: gain_value = 64; break;
        case GAIN_128: gain_value = 128; break;
    }

    float real_amplitude;
    if (overflow) {
        // 找到过零点（1.5V，约 2048）
        int16_t zero_cross_idx = -1;
        for (uint16_t i = 0; i < size - 1; i++) {
            if ((data[i] <= 2048 && data[i + 1] >= 2048) || (data[i] >= 2048 && data[i + 1] <= 2048)) {
                zero_cross_idx = i;
                break;
            }
        }

        if (zero_cross_idx != -1) {
            // 计算实际斜率
            float delta_v = (data[zero_cross_idx + 1] - data[zero_cross_idx]) * 3.0f / 4095.0f;
            float dt = 1.0f / sample_rate;
            float actual_slope = delta_v / dt;

            // 标准斜率 (sin(2.8125°) / dt)
            float angle_step = 2.0f * M_PI / SINE_POINTS; // 2.8125° 转换为弧度
            float std_slope = sinf(angle_step) / dt; // 单位幅度斜率

            // 预测实际幅度
            real_amplitude = fabsf(actual_slope / std_slope) * gain_value;
        } else {
            real_amplitude = 3.0f * gain_value; // 假设最大幅度
        }
    } else {
        real_amplitude = amplitude * gain_value;
    }

    // 选择最优增益（目标 0.5V-2.5V）
    PGA_Gain_t new_gain = GAIN_1;
    if (real_amplitude > 2.5f) {
        new_gain = (PGA_Gain_t)(log2f(2.0f / real_amplitude) + current_gain);
    } else if (real_amplitude < 0.5f) {
        new_gain = (PGA_Gain_t)(log2f(2.0f / real_amplitude) + current_gain);
    }

    if (new_gain > GAIN_128) new_gain = GAIN_128;
    if (new_gain < GAIN_1) new_gain = GAIN_1;

    return new_gain;
}
#endif

#include "pid.h"
#include "boost_adc.h"
#include "io_control.h"
#include "pwm.h"
#include <math.h>

#define READY_THRESH  1.0f
#define READY_COUNT   250

PID_Controller pid = {0};

void PID_Init(float Kp, float Ki, float Kd) {
    pid.Kp = Kp;
    pid.Ki = Ki;
    pid.Kd = Kd;
    pid.target_voltage = 30.0f; // 默认目标电压
		pid.maxduty = 20;
}

void PID_Update(void) {
		if(!Boost_Enable)
		{
			pid.pwm_duty = 0;
			PWM_SetDuty((uint16_t)pid.pwm_duty);
			return;
		}
	  static uint16_t LoadSwitch_Counter=0;  //用于负载打开的计时
    pid.current_voltage = Boost_ADC_ReadVoltage();
    float error = pid.target_voltage - pid.current_voltage;
    
    // 增量式PID计算
    float delta = pid.Kp * (error - pid.error_prev) 
                + pid.Ki * error 
                + pid.Kd * (error - 2*pid.error_prev + pid.error_prev2);
    
    pid.error_prev2 = pid.error_prev;
    pid.error_prev = error;
    
    float new_duty = pid.pwm_duty + delta;
		
		uint16_t maxduty=PWM_GetMaxDuty();
		if(new_duty < 0){
			new_duty = 0;
		}else if(new_duty > maxduty)
		{
			new_duty = maxduty;
		}
    //new_duty = (new_duty < 0) ? 0 : (new_duty > PWM_GetMaxDuty()) ? PWM_GetMaxDuty() : new_duty;
    pid.pwm_duty = new_duty;
    
    PWM_SetDuty((uint16_t)pid.pwm_duty);
    
    if (fabs(error) <= READY_THRESH) {
				if(!pid.is_ready)
				{
					if (pid.ready_counter >= READY_COUNT) {
							pid.is_ready = 1;		//电压到达设定目标值
					}else{
						
						if(pid.ready_counter<2)
						{
							Load_Switch_Set(0,1,1);			//恒流负载
						}
						pid.ready_counter++;
					}
				}
				
    } else {
        pid.ready_counter = 0;
        pid.is_ready = 0;
				if((fabs(error) > 5)&&(error<0)&&(pid.current_voltage>30))
				{
					if((LoadSwitch_Counter%10)==0)
					{
						Load_Switch_Set(1,1,1);			//2K电阻放电+恒流负载  10%的时间, 加速电压到达设定值
					}else if((LoadSwitch_Counter%10)==1)
					{
						Load_Switch_Set(0,1,1);			//恒流负载  90%的时间
					}
					//
					LoadSwitch_Counter++;
					if(LoadSwitch_Counter > 1000)
					{
						LoadSwitch_Counter = 0;
					}
				}else{
					Load_Switch_Set(0,1,1);			//恒流负载  90%的时间
				}
    }
		
}

void PID_SetTargetVoltage(float voltage) {
    if (voltage >= 40.0f && voltage <= 150.0f) {
        pid.target_voltage = voltage;
        pid.ready_counter = 0;
    }
}

uint8_t PID_IsReady(void) {
    return pid.is_ready;
}

uint16_t PID_GetCurrentDuty(void) {
    return pid.pwm_duty;
}

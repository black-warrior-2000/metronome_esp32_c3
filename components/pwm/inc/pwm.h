#ifndef PWM_H
#define PWM_H

#include "driver/ledc.h"
#include "esp_log.h"

// 模式宏定义
#define PWM_MODE_LOW   LEDC_LOW_SPEED_MODE
#define PWM_MODE_HIGH  LEDC_HIGH_SPEED_MODE


#ifdef __cplusplus
extern "C" {
#endif

// PWM初始化（需传入配置参数）
void pwm_init(ledc_channel_t channel, ledc_timer_t timer, 
              ledc_mode_t mode, uint32_t freq_hz, 
              gpio_num_t gpio, uint32_t duty);

// 动态设置占空比（0~100%）
void pwm_set_duty_percent(uint8_t percent);

#ifdef __cplusplus
}
#endif

#endif
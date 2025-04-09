#ifndef PWM_H
#define PWM_H

#include "driver/ledc.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// 模式宏定义
#define PWM_MODE_LOW   LEDC_LOW_SPEED_MODE
#define PWM_MODE_HIGH  LEDC_HIGH_SPEED_MODE

//#define IRQ_ENABLE     1
#define PWM_INIT_ENABLE 1
//#define PWM_FUNC_ENABLE 1

#define PWM_CHANNEL_NUM   1
#define PWM_GPIO_PIN      GPIO_NUM_0

#ifdef __cplusplus
extern "C" {
#endif

void ledc_init(void);
void ledc_set_frequency(void);
void ledc_get_frequency(void);

#ifdef __cplusplus
}
#endif

#endif
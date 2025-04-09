#ifndef __ENENCODER_KEY_H__

#define __ENENCODER_KEY_H__

#include "stdint.h"
#include "stdbool.h"
#include "esp_system.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "driver/gptimer.h"

#define SET  1
#define RESET  0

#define RCC_GPIO            RCC_APB2Periph_GPIOA
#define PORT_GPIO           GPIOA

//SW引脚
#define GPIO_ENCODER_SW     GPIO_NUM_5

//CLK引脚
#define GPIO_ENCODER_CLK    GPIO_NUM_3

//DT引脚
#define GPIO_ENCODER_DT     GPIO_NUM_4

//获取CLK引脚的状态
#define GET_CLK_STATE   gpio_get_level(GPIO_ENCODER_CLK)
//获取DT引脚的状态
#define GET_DT_STATE    gpio_get_level(GPIO_ENCODER_DT)
//获取SW引脚的状态
#define GET_SW_STATE    gpio_get_level(GPIO_ENCODER_SW)


typedef bool FlagStatus;



void Encoder_Init(void);
char Encoder_Scan(void);
unsigned char Encoder_Sw_Down(void);
int Encoder_Rotation_left(void);
void Encoder_Key_Down(void);
int Encoder_Rotation_right(void);

#if 0
char Encoder_Scan(void);
#endif








#endif


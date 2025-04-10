#include "pwm.h"
#include "driver/ledc.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define LEDC_PWM_MODE
//#define GENERATE_PWM_MODE


#ifdef GENERATE_PWM_MODE


#endif


#ifdef LEDC_PWM_MODE
// ledc_timer_config_t ledc_timer = {
//     .speed_mode       = LEDC_LOW_SPEED_MODE,
//     .duty_resolution  = ledc_find_suitable_duty_resolution(),
// }
// };


/*
 * @brief LEDC PWM
 * 
 */
void ledc_init(void)
{
   //LEDC PWM 定时器结构体配置 ledc_timer
    ledc_timer_config_t ledc_timer = {
    .speed_mode       = LEDC_LOW_SPEED_MODE, //定时器模式  低速
    .timer_num        = LEDC_TIMER_0,        //设置定时器源0（0-3）
    .duty_resolution  = LEDC_TIMER_13_BIT,   //将占空比分辨率设置为 13 位  (1-15)
    .freq_hz          = 1000,                //将pwm输出频率设置   1kHz
    .clk_cfg          = LEDC_AUTO_CLK        //配置LEDC时钟源，自动选择
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));//配置LEDC定时器
    
    //LEDC PWM 通道结构体配置  ledc_channel
    ledc_channel_config_t ledc_channel = {    
        .speed_mode     = LEDC_LOW_SPEED_MODE,
        .channel        = LEDC_CHANNEL_0,    //通道0
        .timer_sel      = LEDC_TIMER_0,     
        .intr_type      = LEDC_INTR_DISABLE, //失能LEDC中断
        .gpio_num       = 6, //输出GPIO6
        .duty           = 8191 ,  //将占空比设置为 0%
        .hpoint         = 8191    //LEDC通道的hpoint，最大值为0xfffff
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
}


void ledc_get_frequency(void) {
    uint32_t freq = ledc_get_freq(LEDC_LOW_SPEED_MODE, LEDC_TIMER_0);
    printf("ledc_get_frequency :freq = %lu \r\n", freq);
}

void ledc_set_frequency(void) {
    ledc_set_freq(LEDC_LOW_SPEED_MODE,LEDC_TIMER_0,5000);
}

void ledc_set_frequency_para(int freq) {
    ledc_set_freq(LEDC_LOW_SPEED_MODE,LEDC_TIMER_0,freq);
}


/*
 * @brief 呼吸效果 渐亮到渐灭
 * 
 */
// void Breath_Blink(void)
// {
//     // 初始化淡入淡出服务
//     ledc_fade_func_install(0);    // 注册LEDC服务，在调用前使用，参数是作为是否允许中断

//     //参数：指定速度模式  LEDC通道  衰落的目标占空比 衰落的最长时间（ms）
//     ledc_set_fade_with_time(LEDC_MODE,LEDC_CHANNEL, 8192,500);//在有限的时间内设置LEDC淡入功能
//     //参数：指定速度模式  LEDC通道  淡入淡出功能
//     ledc_fade_start(LEDC_MODE,LEDC_CHANNEL, LEDC_FADE_NO_WAIT);//开始LEDC衰落

//     vTaskDelay(200);
  
//     ledc_set_fade_with_time(LEDC_MODE,LEDC_CHANNEL, 0,500);
//     ledc_fade_start(LEDC_MODE,LEDC_CHANNEL, LEDC_FADE_NO_WAIT);
// }

#endif



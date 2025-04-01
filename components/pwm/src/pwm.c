#include "pwm.h"
#include "driver/ledc.h"
#include "driver/gpio.h"
#include "esp_log.h"

static const char *TAG = "PWM_Component";


static ledc_channel_config_t s_channel;
static ledc_timer_config_t s_timer;
static gpio_num_t s_int_gpio;

// 中断服务例程（翻转IO）
static void IRAM_ATTR pwm_irq_handler(void* arg) {
    ESP_LOGI(TAG, "Interrupt triggered: Toggling PWM GPIO");
    int current_level = gpio_get_level(s_channel.gpio_num);
    gpio_set_level(s_channel.gpio_num, !current_level);
    ESP_LOGD(TAG, "PWM GPIO %d now at level %d", 
             s_channel.gpio_num, !current_level);
}

void pwm_init(ledc_channel_t channel, ledc_timer_t timer, 
              ledc_mode_t mode, uint32_t freq_hz, 
              gpio_num_t gpio, uint32_t duty)
{
    ESP_LOGI(TAG, "Initializing PWM with GPIO %d and Interrupt GPIO %d", 
        pwm_gpio, int_gpio);
        // 配置中断GPIO
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << int_gpio),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .intr_type = GPIO_INTR_ANYEDGE, // 边沿触发
    };
    ESP_ERROR_CHECK(gpio_config(&io_conf));
    ESP_LOGD(TAG, "Interrupt GPIO %d configured", int_gpio);

    // 注册中断服务
    ESP_ERROR_CHECK(gpio_install_isr_service(0));
    ESP_ERROR_CHECK(gpio_isr_handler_add(int_gpio, pwm_irq_handler, NULL));
    s_int_gpio = int_gpio;
    ESP_LOGI(TAG, "Interrupt handler registered for GPIO %d", int_gpio);

    // 1. 配置定时器
    s_timer.speed_mode   = mode;
    s_timer.duty_resolution = LEDC_TIMER_13_BIT;
    s_timer.timer_sel    = timer;
    s_timer.freq_hz      = freq_hz;
    ESP_ERROR_CHECK(ledc_timer_config(&s_timer));
    ESP_LOGI(TAG, "Timer configured: mode %s, freq %dHz", 
             (mode == PWM_MODE_LOW ? "LOW" : "HIGH"), freq_hz);

    // 2. 配置通道
    s_channel.gpio_num   = gpio;
    s_channel.speed_mode = mode;
    s_channel.channel    = channel;
    s_channel.timer_sel  = timer;
    s_channel.duty       = duty;
    ESP_ERROR_CHECK(ledc_channel_config(&s_channel));
    ESP_LOGI(TAG, "PWM channel configured on GPIO %d", pwm_gpio);

    ledc_set_duty(mode, channel, duty);
    ledc_update_duty(mode, channel);
    ESP_LOGD(TAG, "Initial duty set to %d", duty);
}

void pwm_set_duty_percent(uint8_t percent) {
    uint32_t duty_value = (percent * LEDC_DUTY_MAX(LEDC_TIMER_13_BIT)) / 100;
    ESP_LOGI(TAG, "Setting duty to %d%% (%d)", percent, duty_value);
    ledc_set_duty(s_channel.speed_mode, s_channel.channel, duty_value);
    ledc_update_duty(s_channel.speed_mode, s_channel.channel);
    ESP_LOGD(TAG, "Duty updated successfully");
}
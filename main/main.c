/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h"

/* Component  */
#include "pwm.h"
#include "key.h"
#include "encoder_key.h"
extern QueueHandle_t encoder_queue;
void app_main(void)
{
    static char dat = 0;
    encoder_queue = xQueueCreate(10, sizeof(int));
    int encoder_key_state = 0;
    printf("Hello world!\n");

    /* Print chip information */
    esp_chip_info_t chip_info;
    uint32_t flash_size;
    esp_chip_info(&chip_info);
    printf("This is %s chip with %d CPU core(s), %s%s%s%s, ",
           CONFIG_IDF_TARGET,
           chip_info.cores,
           (chip_info.features & CHIP_FEATURE_WIFI_BGN) ? "WiFi/" : "",
           (chip_info.features & CHIP_FEATURE_BT) ? "BT" : "",
           (chip_info.features & CHIP_FEATURE_BLE) ? "BLE" : "",
           (chip_info.features & CHIP_FEATURE_IEEE802154) ? ", 802.15.4 (Zigbee/Thread)" : "");

    unsigned major_rev = chip_info.revision / 100;
    unsigned minor_rev = chip_info.revision % 100;
    printf("silicon revision v%d.%d, ", major_rev, minor_rev);
    if(esp_flash_get_size(NULL, &flash_size) != ESP_OK) {
        printf("Get flash size failed");
        return;
    }

    printf("%" PRIu32 "MB %s flash\n", flash_size / (uint32_t)(1024 * 1024),
           (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

    printf("Minimum free heap size: %" PRIu32 " bytes\n", esp_get_minimum_free_heap_size());

    ledc_init();
    printf("pwm init \r\n");
    Encoder_Init();
    printf("timer init \r\n");


    while(1){
         //vTaskDelay(1000 / portTICK_PERIOD_MS);
         if (xQueueReceive(encoder_queue, &encoder_key_state, pdMS_TO_TICKS(2000))) {
            //printf("encoder_queue receive\n");
            dat = Encoder_Scan();//扫描编码器是否扭动
            if( dat != 0 )//如果有转动
            {
                if( dat == 2 )
                {
                    //Encoder_Rotation_right();
                    ledc_set_frequency_para(Encoder_Rotation_right());
                }
                else
                {
                    //Encoder_Rotation_left();
                    ledc_set_frequency_para(Encoder_Rotation_left());
                }

            }
         }

        // printf("blockyang test\n");
        //ledc_get_frequency();
    }
    // should not run below code
    for (int i = 10; i >= 0; i--) {
        printf("Restarting in %d seconds...\n", i);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    printf("Restarting now.\n");
    fflush(stdout);
    esp_restart();
}

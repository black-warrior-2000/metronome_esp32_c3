#include "encoder_key.h"

static const char *TAG = "encoder_key";

typedef struct {
    uint64_t event_count;
} example_queue_element_t;


static bool example_timer_on_alarm_cb(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx)
{
    BaseType_t high_task_awoken = pdFALSE;
    QueueHandle_t queue = (QueueHandle_t)user_ctx;
    // Retrieve the count value from event data
    example_queue_element_t ele = {
        .event_count = edata->count_value
    };
    // Optional: send the event data to other task by OS queue
    // Don't introduce complex logics in callbacks
    // Suggest dealing with event data in the main loop, instead of in this callback
    xQueueSendFromISR(queue, &ele, &high_task_awoken);
    // return whether we need to yield at the end of ISR
    return high_task_awoken == pdTRUE;
}

static bool encoder_timer_cb(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data)
{
    static char dat = 0;
    dat = Encoder_Scan();//扫描编码器是否扭动
    if( dat != 0 )//如果有转动
    {
        if( dat == 2 )
        {
            Encoder_Rotation_left();
        }
        else
        {
            Encoder_Rotation_right();
        }

    }
    return pdTRUE;
}

static bool encoder_timer_cb_key(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data)
{
    static char dat = 0;
    dat = Encoder_Sw_Down();//扫描编码器按键是否按下
    if( dat != 0 )//如果有转动
    {
            Encoder_Key_Down();
    }
    return pdTRUE;
}




void Encoder_Init(void)
{   

ESP_LOGI(TAG, "Create timer handle");
gptimer_handle_t gptimer = NULL;
gptimer_config_t timer_config = {
    .clk_src = GPTIMER_CLK_SRC_DEFAULT,
    .direction = GPTIMER_COUNT_UP,
    .resolution_hz = 100000, // 0.1MHz, 1 tick=10us
};
ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &gptimer));

gptimer_event_callbacks_t cbs = {
    .on_alarm = encoder_timer_cb,
};
ESP_ERROR_CHECK(gptimer_register_event_callbacks(gptimer, &cbs, NULL));

ESP_LOGI(TAG, "Enable gptimer");
ESP_ERROR_CHECK(gptimer_enable(gptimer));

ESP_LOGI(TAG, "Start gptimer, stop it at alarm event");
gptimer_alarm_config_t alarm_config1 = {
    .reload_count = 0,
    .alarm_count = 1000, // period = 0.01s=10ms
    .flags.auto_reload_on_alarm = true,
};
ESP_ERROR_CHECK(gptimer_set_alarm_action(gptimer, &alarm_config1));
ESP_ERROR_CHECK(gptimer_start(gptimer));

#if 0
gptimer_handle_t gptimer_key = NULL;
gptimer_config_t timer_config_key = {
    .clk_src = GPTIMER_CLK_SRC_DEFAULT,
    .direction = GPTIMER_COUNT_UP,
    .resolution_hz = 100000, // 0.1MHz, 1 tick=10us
};
ESP_ERROR_CHECK(gptimer_new_timer(&timer_config_key, &gptimer_key));
gptimer_event_callbacks_t cbs_key = {
    .on_alarm = encoder_timer_cb_key,
};
ESP_ERROR_CHECK(gptimer_register_event_callbacks(gptimer_key, &cbs_key, NULL));

ESP_LOGI(TAG, "Enable gptimer_key");
ESP_ERROR_CHECK(gptimer_enable(gptimer_key));

ESP_LOGI(TAG, "Start gptimer_key, stop it at alarm event");
gptimer_alarm_config_t alarm_config2 = {
    .reload_count = 0,
    .alarm_count = 1000, // period = 0.01s=10ms
    .flags.auto_reload_on_alarm = true,
};

ESP_ERROR_CHECK(gptimer_set_alarm_action(gptimer_key, &alarm_config2));
ESP_ERROR_CHECK(gptimer_start(gptimer_key));
#endif

};


// void Encoder_Deinit(void){
//     ESP_ERROR_CHECK(gptimer_stop(gptimer));
// }






/******************************************************************
 * 函 数 名 称：Encoder_Scanf
 * 函 数 说 明：判断旋转编码器是否有往哪一个方向旋转
 * 函 数 形 参：无
 * 函 数 返 回：1=正转 2=反转
 * 作       者：LC
 * 备       注：哪一边正转哪一边反转不需要太在意，你说的算
******************************************************************/
char Encoder_Scan(void)
{
    static  FlagStatus    EC11_CLK_Last= RESET;     //EC11的LCK引脚上一次的状态 （A相）
    static  FlagStatus    EC11_DT_Last = RESET;     //EC11的DT引脚上一次的状态（B相）
    char ScanResult = 0;
    //当A发生跳变时采集B当前的状态，并将B与上一次的状态进行对比。
    if(GET_CLK_STATE !=EC11_CLK_Last)
    {           //若A 0->1 时，B 1->0 正转；若A 1->0 时，B 0->1 正转；
                //若A 0->1 时，B 0->1 反转；若A 1->0 时，B 1->0 反转
        if(GET_CLK_STATE == 1)     //EC11_A和上一次状态相比，为上升沿
        {
            //EC11_B和上一次状态相比，为下降沿
            if((EC11_DT_Last == 1)&&(GET_DT_STATE == 0))
                ScanResult = 1;  //正转
             //EC11_B和上一次状态相比，为上升沿
            if((EC11_DT_Last == 0)&&(GET_DT_STATE == 1))
                ScanResult = 2; //反转

            //>>>>>>>>>>>>>>>>下面为正转一次再反转或反转一次再正转处理<<<<<<<<<<<<<<<<//
            //A上升沿时，采集的B不变且为0
            if((EC11_DT_Last == GET_DT_STATE)&&(GET_DT_STATE == 0))
                ScanResult = 1;                                 //正转
             //A上升沿时，采集的B不变且为1
            if((EC11_DT_Last == GET_DT_STATE)&&(GET_DT_STATE == 1))
                ScanResult = 2;                                //反转
        }
        else  //EC11_A和上一次状态相比，为下降沿
        {
            //EC11_B和上一次状态相比，为下降沿
            if((EC11_DT_Last == 1)&&(GET_DT_STATE == 0))
                ScanResult = 2;                        //反转
             //EC11_B和上一次状态相比，为上升沿
            if((EC11_DT_Last == 0)&&(GET_DT_STATE == 1))
                ScanResult = 1;                         //正转

            //>>>>>>>>>>>>>>>>下面为正转一次再反转或反转一次再正转处理<<<<<<<<<<<<<<<<//
            //A上升沿时，采集的B不变且为0
            if((EC11_DT_Last == GET_DT_STATE)&&(GET_DT_STATE == 0))
                ScanResult = 2;                                //反转
            //A上升沿时，采集的B不变且为1
            if((EC11_DT_Last == GET_DT_STATE)&&(GET_DT_STATE == 1))
                ScanResult = 1;                                 //正转
        }
        EC11_CLK_Last = GET_CLK_STATE;   //更新编码器上一个状态暂存变量
        EC11_DT_Last = GET_DT_STATE;     //更新编码器上一个状态暂存变量
        return ScanResult;               //返回值的取值：   0：无动作； 1：正转；  2：反转；
    }
    return 0;
}

/******************************************************************
 * 函 数 名 称：Encoder_Sw_Down
 * 函 数 说 明：判断编码器是否被按下
 * 函 数 形 参：无
 * 函 数 返 回：0=没有被按下  1=被按下
 * 作       者：LC
 * 备       注：请注意消抖
******************************************************************/
unsigned char Encoder_Sw_Down(void)
{
    //没有按下
    if( GET_SW_STATE == SET )
    {
        vTaskDelay(100 / portTICK_PERIOD_MS);//消抖
        return 0;
    }
    else//按下
    {
        vTaskDelay(100 / portTICK_PERIOD_MS);//消抖
        printf("key down\r\n");
        return 1;
    }
}

/******************************************************************
 * 函 数 名 称：Encoder_Rotation_left
 * 函 数 说 明：左旋转服务函数。当编码器左转时，需要执行的操作
 * 函 数 形 参：无
 * 函 数 返 回：向左旋转次数
 * 作       者：LC
 * 备       注：无
******************************************************************/
int Encoder_Rotation_left(void)
{
    static int left_num = 0;//左转次数
    left_num++;
    /*  你的代码写在此处  */
    //printf("left num = %d\r\n",left_num);

    /*  你的代码写在此处  */
    return left_num;
}

void Encoder_Key_Down(void)
{
    //printf("key down\r\n");
}

/******************************************************************
 * 函 数 名 称：Encoder_Rotation_left
 * 函 数 说 明：右旋转服务函数。当编码器右转时，需要执行的操作
 * 函 数 形 参：无
 * 函 数 返 回：向右旋转次数
 * 作       者：LC
 * 备       注：无
******************************************************************/
int Encoder_Rotation_right(void)
{
    static int right_num = 0;//右转次数
    right_num++;
    /*  你的代码写在此处  */

    //printf("right num = %d\r\n",right_num);
    /*  你的代码写在此处  */

    return right_num;
}






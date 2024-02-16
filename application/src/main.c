#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/pwm.h"
#include "hardware/flash.h"
#include "pwm.pio.h"
//#include "pico/cyw43_arch.h"
#include "pwm.h"
#include "pwm_pio.h"
#include "config.h"
#include "my_i2c.h"
#include "io_exp.h"
#include "utils.h"
#include "rtt/RTT/SEGGER_RTT.h"
#include "my_flash.h"
#include "effects.h"

#include "btstack.h"
#include "pico/cyw43_arch.h"
#include "pico/btstack_cyw43.h"
#include "server_common.h"

#define HEARTBEAT_PERIOD_MS 1000


struct repeating_timer main_timer;
struct repeating_timer io_pool_timer;

static uint32_t io_exp_pooling_counter = 0;
static volatile bool io_exp_pooling_flag = false;


bool main_timer_callback(struct repeating_timer *t) // each 2ms
{
    // timer do poolidngu IO expandera
    io_exp_pooling_counter ++;
    if ( io_exp_pooling_counter > 50 )
    {
        io_exp_pooling_flag = true;
        io_exp_pooling_counter = 0;
    }

    // Effect main loop (function pointer)
    stair_effect_event.effect_loop();

    // Condition to turn on light
    if ( ( light_on.light_on_flag == true ) && ( ( light_on.light_on_from_top_flag == true ) || ( light_on.light_on_from_bottom_flag == true ) ) )
    {
        light_on.light_off_counter_ms = light_on.light_off_counter_ms * MAIN_TIMER_LOOP_MS;
        if ( light_on.light_off_counter_ms > settings.stair_light_on_time_ms/2 ) // time diveded by 2 because loop interval is 2ms
        {
            SEGGER_RTT_WriteString(0,"Light exit\r\n");
            printf("Light exit\r\n");
            light_on.dir == DIR_UP_TO_DOWN ? stair_effect_event.effect_end(DIR_UP_TO_DOWN) : stair_effect_event.effect_end(DIR_DOWN_TO_UP);
            light_on.light_on_flag = false;
            light_on.light_off_counter_ms = 0;
        }  
    }   

    return true;
}


void sens_top_enter(void)
{
    SEGGER_RTT_WriteString(0,"Sensor top enter\r\n");   
    printf("Sensor top enter\r\n"); 
    light_on.light_off_counter_ms = 0;
    light_on.light_on_from_top_flag = false;
    if ( ( light_on.light_on_from_top_flag == false ) && ( light_on.light_on_flag == false ) )
    {
        SEGGER_RTT_WriteString(0,"Light enter\r\n");
        printf("Light enter\r\n");
        light_on.light_on_flag = true;
        light_on.dir = DIR_UP_TO_DOWN;
        stair_effect_event.effect_start(light_on.dir);
    }  
}

void sens_top_exit(void)
{
    light_on.light_on_from_top_flag = true;
    SEGGER_RTT_WriteString(0,"Sensor top exit\r\n");   
    printf("Sensor top exit\r\n");   
}

void action_sensor_top(void)
{
    printf("Action: TOP");
    SEGGER_RTT_WriteString(0,"Sensor top enter\r\n");   
    light_on.light_off_counter_ms = 0;
    light_on.light_on_from_top_flag = false;
    if ( ( light_on.light_on_from_top_flag == false ) && ( light_on.light_on_flag == false ) )
    {
        SEGGER_RTT_WriteString(0,"Light enter\r\n");
        light_on.light_on_flag = true;
        light_on.dir = DIR_UP_TO_DOWN;
        stair_effect_event.effect_start(light_on.dir);
        //effect_1_start(light_on.dir);
    }  
     light_on.light_on_from_top_flag = true;
}



void sens_bottom_enter(void)
{
    SEGGER_RTT_WriteString(0,"Sensor bottom enter\r\n");
    printf("Sensor bottom enter\r\n");
    light_on.light_off_counter_ms = 0;
    light_on.light_on_from_bottom_flag = false;
    if ( ( light_on.light_on_from_bottom_flag == false ) && ( light_on.light_on_flag == false ) )
    {
        SEGGER_RTT_WriteString(0,"Light enter\r\n");
        light_on.light_on_flag = true;
        light_on.dir = DIR_DOWN_TO_UP;

        stair_effect_event.effect_start(light_on.dir);
    }  
}

void sens_bottom_exit(void)
{
    light_on.light_on_from_bottom_flag = true;
    SEGGER_RTT_WriteString(0,"Sensor bottom exit\r\n"); 
    printf("Sensor bottom exit\r\n"); 
}



void action_sensor_bottom(void)
{
    printf("Action: Bottom");
    SEGGER_RTT_WriteString(0,"Sensor bottom enter\r\n");
    light_on.light_off_counter_ms = 0;
    light_on.light_on_from_bottom_flag = false;
    if ( ( light_on.light_on_from_bottom_flag == false ) && ( light_on.light_on_flag == false ) )
    {
        SEGGER_RTT_WriteString(0,"Light enter\r\n");
        light_on.light_on_flag = true;
        light_on.dir = DIR_DOWN_TO_UP;
        stair_effect_event.effect_start(light_on.dir);
    } 
    light_on.light_on_from_bottom_flag = true;
    SEGGER_RTT_WriteString(0,"Sensor bottom exit\r\n"); 
}




// Related to bluetooth
static btstack_timer_source_t heartbeat;
static btstack_packet_callback_registration_t hci_event_callback_registration;

static void heartbeat_handler(struct btstack_timer_source *ts) 
{
    static uint32_t counter = 0;
    counter++;

    // // Update the temp every 10s
    // if (counter % 10 == 0) {
    //     poll_temp();
    //     if (le_notification_enabled) {
    //         att_server_request_can_send_now_event(con_handle);
    //     }
    // }

    // Invert the led
    static int led_on = true;
    led_on = !led_on;
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, led_on);

    // Restart timer
    btstack_run_loop_set_timer(ts, HEARTBEAT_PERIOD_MS);
    btstack_run_loop_add_timer(ts);
}



int main() 
{
    stdio_init_all();
    start_blink_led();
    boot_info_rtt();

    //SETTINGS_load_and_save_default();
    SETTINGS_load_and_increase_reset_count();

    PWM_HW_init();
    PWM_PIO_init();
    I2C_init();
    IO_EXP_init();
    IO_EXP_reg_event_sens_top_cbfunc(sens_top_enter, sens_top_exit);
    IO_EXP_reg_event_sens_bottom_cbfunc(sens_bottom_enter, sens_bottom_exit);

    add_repeating_timer_us(MAIN_TIMER_LOOP_MS * (-1000), main_timer_callback, NULL, &main_timer);


    if (cyw43_arch_init()) 
    {
        printf("failed to initialise cyw43_arch\n");
        return -1;
    }

    l2cap_init();
    sm_init();

    att_server_init(profile_data, att_read_callback, att_write_callback);    
    // inform about BTstack state
    hci_event_callback_registration.callback = &packet_handler;
    hci_add_event_handler(&hci_event_callback_registration);
    // register for ATT event
    att_server_register_packet_handler(packet_handler);
    // set one-shot btstack timer
    heartbeat.process = &heartbeat_handler;
    btstack_run_loop_set_timer(&heartbeat, HEARTBEAT_PERIOD_MS);
    btstack_run_loop_add_timer(&heartbeat);

    // turn on bluetooth!
    hci_power_control(HCI_POWER_ON);
    
    //btstack_run_loop_execute();

    delay_boot_info();

    while(1)
    {
        static uint32_t counter = 0;

        if ( io_exp_pooling_flag == true)
        { 
            io_exp_pooling_flag = false;
            counter++;
            SEGGER_RTT_printf(0,"pooling io exp. C: %4d enable:%d light off counter: %d\r\n", counter, effect_1.enable, light_on.light_off_counter_ms);
            IO_EXP_pooling();
        }
    }
}

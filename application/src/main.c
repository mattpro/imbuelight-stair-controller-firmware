#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/pwm.h"
#include "hardware/flash.h"
#include "hardware/watchdog.h"
#include "pwm.pio.h"
//#include "pico/cyw43_arch.h"
#include "pwm.h"
#include "pwm_pio.h"
#include "config.h"
#include "my_i2c.h"
#include "io_exp.h"
#include "utils.h"
#include "rtt/RTT/SEGGER_RTT.h"
#include "settings.h"
#include "my_rtc.h"
#include "ble_communication.h"
#include "effects.h"

#include "btstack.h"
#include "pico/cyw43_arch.h"
#include "pico/btstack_cyw43.h"
#include "server_common.h"

#define HEARTBEAT_PERIOD_MS 1000


struct repeating_timer main_timer;
struct repeating_timer io_pool_timer;

static volatile uint32_t io_exp_pooling_counter = 0;
static volatile bool io_exp_pooling_flag = false;


bool main_timer_callback(struct repeating_timer *t) // each 2ms
{
    int effect_stat = 0;
    // timer do poolidngu IO expandera
    io_exp_pooling_counter ++;
    if ( io_exp_pooling_counter > 50 )
    {
        io_exp_pooling_flag = true;
        io_exp_pooling_counter = 0;
    }

    if ( manual_brightness_control_flag )
    {
        return true;
    }

    // Effect main loop (function pointer)
    effect_stat = stair_effect_event.effect_loop();

    // Condition to turn on light
    if ( ( effect_stat == EFFECT_OFF ) && ( light_on.light_on_flag == true ) && ( ( light_on.light_on_from_top_flag == true ) || ( light_on.light_on_from_bottom_flag == true ) ) )
    {
        light_on.light_off_counter_ms += MAIN_TIMER_LOOP_MS;
        if ( light_on.light_off_counter_ms > settings.stair_light_on_time_ms ) 
        {
            SEGGER_RTT_WriteString(0,"Light exit\r\n");
            //printf("Light exit\r\n");
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

void sens_bottom_enter(void)
{
    printf("Sensor bottom enter\r\n");
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
}

void sens_bottom_exit(void)
{
    light_on.light_on_from_bottom_flag = true;
    SEGGER_RTT_WriteString(0,"Sensor bottom exit\r\n"); 
    printf("Sensor bottom exit\r\n"); 
}

void action_sensor_top(void)
{
    printf("Action: TOP\r\n");
    SEGGER_RTT_WriteString(0,"Sensor top enter\r\n");   
    light_on.light_off_counter_ms = 0;
    light_on.light_on_from_top_flag = false;
    if ( ( light_on.light_on_from_top_flag == false ) && ( light_on.light_on_flag == false ) )
    {
        SEGGER_RTT_WriteString(0,"Light enter\r\n");
        light_on.light_on_flag = true;
        light_on.dir = DIR_UP_TO_DOWN;
        stair_effect_event.effect_start(light_on.dir);
    }  
     light_on.light_on_from_top_flag = true;
}

void action_sensor_bottom(void)
{
    printf("Action: Bottom\r\n");
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



// // Related to bluetooth
static btstack_timer_source_t heartbeat;
static btstack_packet_callback_registration_t hci_event_callback_registration;

static void heartbeat_handler(struct btstack_timer_source *ts) 
{
    static uint32_t counter = 0;
    counter++;

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

    RTC_init();
    
    IO_EXP_reg_event_sens_top_cbfunc(sens_top_enter, sens_top_exit);
    IO_EXP_reg_event_sens_bottom_cbfunc(sens_bottom_enter, sens_bottom_exit);

    add_repeating_timer_us(MAIN_TIMER_LOOP_MS * (1000), main_timer_callback, NULL, &main_timer);


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

    delay_boot_info();

    // Enable the watchdog, requiring the watchdog to be updated every 100ms or the chip will reboot
    // second arg is pause on debug which means the watchdog will pause when stepping through code
    watchdog_enable(1000, 0);

    // while (1)
    // {
    //     if (le_notification_enabled) {
    //         att_server_request_can_send_now_event(con_handle);
    //     }
    //     watchdog_update();

    //     sleep_ms(200);
    // }
    
    while(1)
    {
        static uint32_t counter = 0;

        watchdog_update();

        if ( io_exp_pooling_flag == true) // 100 ms
        { 
            io_exp_pooling_flag = false;
            counter++;
            //SEGGER_RTT_printf(0,"pooling io exp. C: %4d enable:%d light off counter: %d\r\n", counter, effect_1.enable, light_on.light_off_counter_ms);
            IO_EXP_pooling();

            if ( counter % 10 == 0 )
            {
                RTC_print_date_and_time();
                static int led_on = true;
                led_on = !led_on;
                cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, led_on);
            }
        }

        if ( save_flag == 1 )
        {
            save_flag = 0;
            SETTINGS_save();

            PWM_set_all(settings.max_pwm_duty);
            sleep_ms(100);
            PWM_set_all(0);
            sleep_ms(100);
            PWM_set_all(settings.max_pwm_duty);
            sleep_ms(100);
            PWM_set_all(0);
        }

        if ( connected_flag == 1)
        {
            connected_flag = 0;
            PWM_set_all(settings.max_pwm_duty);
            sleep_ms(200);
            PWM_set_all(0);
        }
    }
}

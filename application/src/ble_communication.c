#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "config.h"
#include "settings.h"
#include "pwm.h"
#include "ble_communication.h"
#include "settings.h"
#include "rtt/RTT/SEGGER_RTT.h"
#include "pico/stdlib.h"


static ble_cmd_t ble_cmd;
static ble_param_t ble_param;

uint8_t manual_brightness_control_flag = 0;

extern void action_sensor_top(void);
extern void action_sensor_bottom(void);


static void BLE_COM_parse_parameter_set(uint8_t* data, uint16_t len);
static void BLE_COM_parse_parameter_action(uint8_t* data, uint16_t len);


void BLE_COM_parse(uint8_t* data, uint16_t len)
{
    if ( len < 2 )
    {
        SEGGER_RTT_WriteString(0, "BLE COM: data len too shortv");
        return;
    }
    ble_cmd = (ble_cmd_t)(data[0]);
    printf("Recived new BLE");
    printf("BLE CMD: %02X\r\n", data[0]);
    printf("BLE PAR: %02X\r\n", data[1]);

    for ( int i = 0 ; i < (len - 2) ; i ++ )
    {
        printf("BLE DAT[%d]: %02X\r\n",i, data[2+i]); 
    }
    
    switch(ble_cmd)
    {
        case COMMAND_SET_PARAMETER:
            SEGGER_RTT_WriteString(0, "BLE COM: CMD=SET_PARAMETER\r\n");
            BLE_COM_parse_parameter_set(&data[1], len - 1);
        break;
        case COMMAND_ACTION:
            SEGGER_RTT_WriteString(0, "BLE COM: CMD=SET_ACTION\r\n");
            BLE_COM_parse_parameter_action(&data[1], len - 1);
        break;
        case COMMAND_ADDITIONAL:
            SEGGER_RTT_WriteString(0, "BLE COM: CMD=SET_ADDITIONAL\r\n");

        break;
        case COMMAND_CONTROLL:
            SEGGER_RTT_WriteString(0, "BLE COM: CMD=SET_CONTROLL\r\n");


        break;
        default:
            SEGGER_RTT_WriteString(0, "BLE COM: CMD=SET Unknow\r\n");
        break;
    }
}


static void BLE_COM_parse_parameter_set(uint8_t* data, uint16_t len)
{
    ble_param = (ble_cmd_t)(data[0]);

    switch(ble_param)
    {
        case PARAM_SET_NUM_STAIR:
            SEGGER_RTT_WriteString(0, "BLE COM: PARAM=SET_NUM_STAIR\r\n");
            if ( data[1] < MAX_NUM_STAIR )
            {
                settings.num_of_stairs = data[1];
                SEGGER_RTT_printf(0, "BLE COM: SET_NUM_STAIR=%d\r\n", settings.num_of_stairs);
                printf("BLE COM: SET_NUM_STAIR=%d\r\n", settings.num_of_stairs);
            }
            else
            {
                SEGGER_RTT_WriteString(0, "BLE COM: Wrong stair number\r\n");
                printf("BLE COM: Wrong stair number\r\n");
            }
        break; 
        case PARAM_SET_MAX_PWM:
            SEGGER_RTT_WriteString(0, "BLE COM: PARAM=SET_MAX_PWM\r\n");
            uint16_t max_pwm;
            max_pwm = ( (uint16_t)data[1] << 8 ) | data[2];
            SEGGER_RTT_printf(0, "BLE COM: SET_MAX_PWM=%d\r\n", max_pwm);
            printf("BLE COM: SET_MAX_PWM=%d\r\n", max_pwm);
            settings.max_pwm_duty = max_pwm;
        break;
        case PARAM_SET_LIGHT_TIME:
            SEGGER_RTT_WriteString(0, "BLE COM: PARAM=SET_LIGHT_TIME\r\n");
            uint32_t stair_light_on_time_ms;
            stair_light_on_time_ms = ( (uint16_t)data[1] << 8 ) | ( (uint16_t)data[2] << 8 ) | ( (uint16_t)data[3] << 8 ) | data[4];
            SEGGER_RTT_printf(0, "BLE COM: SET_LIGHT_TIME=%d\r\n", stair_light_on_time_ms);
            printf("BLE COM: SET_MAX_PWM=%d\r\n", max_pwm);
            settings.stair_light_on_time_ms = stair_light_on_time_ms;
        break; 
        case PARAM_SET_EFFECT:
            SEGGER_RTT_WriteString(0, "BLE COM: PARAM=SET_EFFECT\r\n");
            uint8_t effect_number = data[1];
            if ( ( effect_number == 1 ) || ( effect_number == 2 ) ) // We alredy have only two effect. TODO: need refactor
            {
                settings.selected_effect_number = effect_number;
                SEGGER_RTT_printf(0, "BLE COM: SET_EFFECT=%d\r\n", effect_number);
                printf("BLE COM: SET_EFFECT=%d\r\n", effect_number);
            }
            else
            {
                SEGGER_RTT_printf(0, "Wrong effect number\r\n");
                printf("Wrong effect number\r\n");
            }

            if ( effect_number == 1 )
            {
                settings.effect1_settings_1 =  ( (uint16_t)data[2] << 8 ) | data[3];
                settings.effect1_settings_2 =  ( (uint16_t)data[4] << 8 ) | data[5];
                SEGGER_RTT_printf(0, "BLE COM: effect1_settings_1=%d\r\n", settings.effect1_settings_1 );
                SEGGER_RTT_printf(0, "BLE COM: effect1_settings_2=%d\r\n", settings.effect1_settings_2 );
                printf("BLE COM: effect1_settings_1=%d\r\n", settings.effect1_settings_1 );
                printf("BLE COM: effect1_settings_2=%d\r\n", settings.effect1_settings_2 );
            }
            if ( effect_number == 2 ) // Effect 2 have only one setting
            {
                settings.effect2_settings_1 =  ( (uint16_t)data[2] << 8 ) | data[3];
                SEGGER_RTT_printf(0, "BLE COM: effect2_settings_1=%d\r\n", settings.effect2_settings_1 );
                printf("BLE COM: effect1_settings_2=%d\r\n", settings.effect1_settings_1 );
            }
        break; 
        case PARAM_SET_ALL_PARAM:
            SEGGER_RTT_WriteString(0, "BLE COM: PARAM=SET_ALL_PARAM\r\n");
            printf("BLE COM: PARAM=SET_ALL_PARAM\r\n");
        break; 
        case PARAM_SET_LOAD_AND_SAVE_DEFAULT:
            SEGGER_RTT_WriteString(0, "BLE COM: PARAM=PARAM_SET_LOAD_AND_SAVE_DEFAULT\r\n");
            printf("BLE COM: PARAM=PARAM_SET_LOAD_AND_SAVE_DEFAULT\r\n");
            SETTINGS_load_and_save_default();
            SETTINGS_print_rtt();
            SETTINGS_print_serial();
        break;
        case PARAM_SET_SAVE_ALL:
            SEGGER_RTT_WriteString(0, "BLE COM: PARAM=PARAM_SAVEL_ALL\r\n");
            printf("BLE COM: PARAM=PARAM_SAVEL_ALL\r\n");
            SETTINGS_print_rtt();
            SETTINGS_print_serial();
            SETTINGS_save();
        break;
        default:
            SEGGER_RTT_WriteString(0, "BLE COM: PARAM=Unknow\r\n");
            printf("BLE COM: PARAM=Unknow\r\n");
        break;
    }    
}


static void BLE_COM_parse_parameter_action(uint8_t* data, uint16_t len)
{
    uint16_t manual_brightness;

    ble_param = (ble_cmd_t)(data[0]);

    switch(ble_param)
    {
        case PARAM_ACT_PRESS_SW_1:
            SEGGER_RTT_WriteString(0, "BLE COM: PARAM=ACT_PRESS_SW_1\r\n");
            printf("BLE COM: PARAM=ACT_PRESS_SW_1\r\n");
        break; 
        case PARAM_ACT_PRESS_SW_2:
            SEGGER_RTT_WriteString(0, "BLE COM: PARAM=ACT_PRESS_SW_2\r\n");
            printf("BLE COM: PARAM=ACT_PRESS_SW_2\r\n");
        break;
        case PARAM_ACT_PRESS_TOP_PRESS:
            SEGGER_RTT_WriteString(0, "BLE COM: PARAM=ACT_PRESS_TOP_PRESS\r\n");
            printf("BLE COM: PARAM=ACT_PRESS_TOP_PRESS\r\n");
            action_sensor_top();
        break; 
        case PARAM_ACT_PRESS_BOTTOM_PRESS:
            SEGGER_RTT_WriteString(0, "BLE COM: PARAM=ACT_PRESS_BOTTOM_PRESS\r\n");
            printf("BLE COM: PARAM=ACT_PRESS_BOTTOM_PRESS\r\n");
            action_sensor_bottom();
        break; 
        case PARAM_ACT_MANUAL_BRIGHTNESS_CONTROL_ENABLE_DISABLE:
            manual_brightness_control_flag = data[1];
            SEGGER_RTT_printf(0, "BLE COM: PARAM=ACT_MANUAL_BRIGHTNESS_CONTROL_ENABLE_DISABLE: %d\r\n", manual_brightness_control_flag);
            if ( manual_brightness_control_flag == 0 )
            {
                PWM_set_all( 0 );
            }
        break;
        case PARAM_ACT_SET_MANUAL_BRIGHTNESS:
            manual_brightness = ( (uint16_t)data[2] << 8 ) | data[1];
            SEGGER_RTT_printf(0, "Set manual brightness: %d\r\n", manual_brightness);
            if ( manual_brightness_control_flag == 1)
            {
                PWM_set_all( manual_brightness );
            }
        break;
        default:
            SEGGER_RTT_WriteString(0, "BLE COM: PARAM=ACT Unknow\r\n");
            printf("BLE COM: PARAM=ACT Unknow\r\n");
        break;
    }    
}


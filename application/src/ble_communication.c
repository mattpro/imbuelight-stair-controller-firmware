#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "config.h"
#include "my_flash.h"
#include "pwm.h"
#include "ble_communication.h"
#include "rtt/RTT/SEGGER_RTT.h"
#include "my_flash.h"
#include "pico/stdlib.h"


static ble_cmd_t ble_cmd;
static ble_param_t ble_param;


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

        break; 
        case PARAM_SET_ALL_PARAM:
            SEGGER_RTT_WriteString(0, "BLE COM: PARAM=SET_ALL_PARAM\r\n");
        break; 
        case PARAM_SET_LOAD_AND_SAVE_DEFAULT:
            SETTINGS_load_and_save_default();
        break;
        case PARAM_SET_SAVE_ALL:
            SEGGER_RTT_WriteString(0, "BLE COM: PARAM=PARAM_SAVEL_ALL\r\n");
            SETTINGS_print_rtt();
            SETTINGS_print_serial();
            SETTINGS_save();
        break;
        default:
            SEGGER_RTT_WriteString(0, "BLE COM: PARAM=Unknow\r\n");
        break;
    }    
}


static void BLE_COM_parse_parameter_action(uint8_t* data, uint16_t len)
{
    ble_param = (ble_cmd_t)(data[0]);

    switch(ble_param)
    {
        case PARAM_ACT_PRESS_SW_1:
            SEGGER_RTT_WriteString(0, "BLE COM: PARAM=ACT_PRESS_SW_1\r\n");
            printf("ACTION: SW1");
        break; 
        case PARAM_ACT_PRESS_SW_2:
            SEGGER_RTT_WriteString(0, "BLE COM: PARAM=ACT_PRESS_SW_2\r\n");
            printf("ACTION: SW2");
        break;
        case PARAM_ACT_PRESS_TOP_PRESS:
            SEGGER_RTT_WriteString(0, "BLE COM: PARAM=ACT_PRESS_TOP_PRESS\r\n");
            action_sensor_top();
        break; 
        case PARAM_ACT_PRESS_BOTTOM_PRESS:
            SEGGER_RTT_WriteString(0, "BLE COM: PARAM=ACT_PRESS_BOTTOM_PRESS\r\n");
            action_sensor_bottom();
        break; 
        default:
            SEGGER_RTT_WriteString(0, "BLE COM: PARAM=ACT Unknow\r\n");
        break;
    }    
}


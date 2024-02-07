#include <stdint.h>
#include <string.h>
#include "config.h"
#include "my_flash.h"
#include "pwm.h"
#include "ble_communication.h"
#include "rtt/RTT/SEGGER_RTT.h"
#include "pico/stdlib.h"


static ble_cmd_t ble_cmd;
static ble_param_t ble_param;


static void BLE_COM_parse_parameter_set(uint8_t* data, uint16_t len)
{
    ble_param = (ble_cmd_t)(data[0]);

    switch(ble_param)
    {
        case PARAM_SET_NUM_STAIR:
            SEGGER_RTT_WriteString(0, "BLE COM: PARAM=SET_NUM_STAIR\r\n");
            
        break; 
        case PARAM_SET_MAX_PWM:
            SEGGER_RTT_WriteString(0, "BLE COM: PARAM=SET_MAX_PWM\r\n");
        break;
        case PARAM_SET_LIGHT_TIME:
            SEGGER_RTT_WriteString(0, "BLE COM: PARAM=SET_LIGHT_TIME\r\n");
        break; 
        case PARAM_SET_EFFECT:
            SEGGER_RTT_WriteString(0, "BLE COM: PARAM=SET_EFFECT\r\n");
        break; 
        case PARAM_SET_ALL_PARAM:
            SEGGER_RTT_WriteString(0, "BLE COM: PARAM=SET_ALL_PARAM\r\n");
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
        break; 
        case PARAM_ACT_PRESS_SW_2:
            SEGGER_RTT_WriteString(0, "BLE COM: PARAM=ACT_PRESS_SW_2\r\n");
        break;
        case PARAM_ACT_PRESS_TOP_PRESS:
            SEGGER_RTT_WriteString(0, "BLE COM: PARAM=ACT_PRESS_TOP_PRESS\r\n");
        break; 
        case PARAM_ACT_PRESS_BOTTOM_PRESS:
            SEGGER_RTT_WriteString(0, "BLE COM: PARAM=ACT_PRESS_BOTTOM_PRESS\r\n");
        break; 
        default:
            SEGGER_RTT_WriteString(0, "BLE COM: PARAM=ACT Unknow\r\n");
        break;
    }    
}



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


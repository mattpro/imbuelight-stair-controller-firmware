#ifndef _BLE_COMMUNICATION_H_
#define _BLE_COMMUNICATION_H_

#include <stdint.h>


typedef enum { 
    COMMAND_SET_PARAMETER           = 0xDE, 
    COMMAND_ACTION                  = 0xAA, 
    COMMAND_ADDITIONAL              = 0xAF,
} ble_cmd_t;

typedef enum { 
    PARAM_SET_NUM_STAIR             = 0x01,  
    PARAM_SET_MAX_PWM               = 0x02, 
    PARAM_SET_LIGHT_TIME            = 0x03, 
    PARAM_SET_EFFECT                = 0x04, 
    PARAM_SET_ALL_PARAM             = 0xFF, 
    PARAM_SET_LOAD_AND_SAVE_DEFAULT = 0xDF,
    PARAM_SET_SAVE_ALL              = 0xEE,

    PARAM_ACT_PRESS_SW_1            = 0xA1,
    PARAM_ACT_PRESS_SW_2            = 0xB1,
    PARAM_ACT_PRESS_TOP_PRESS       = 0x01,
    PARAM_ACT_PRESS_BOTTOM_PRESS    = 0x02,

    PARAM_ADD_SWITCH_CHANNEL        = 0x01,
} ble_param_t;


void BLE_COM_parse(uint8_t* data, uint16_t len);



#endif

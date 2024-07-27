#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "config.h"
#include "settings.h"
#include "pwm.h"
#include "ble_communication.h"
#include "settings.h"
#include "effects.h"
#include "server_common.h"
#include "rtt/RTT/SEGGER_RTT.h"
#include "pico/stdlib.h"


static ble_cmd_t ble_cmd;
static ble_param_t ble_param;

uint8_t manual_brightness_control_flag = 0;

extern void action_sensor_top(void);
extern void action_sensor_bottom(void);


static void BLE_COM_parse_parameter_set(uint8_t* data, uint16_t len);
static void BLE_COM_parse_parameter_action(uint8_t* data, uint16_t len);
static void BLE_COM_parse_parameter_controll(uint8_t* data, uint16_t len);

void BLE_COM_parse(uint8_t* data, uint16_t len)
{
    if ( len < 2 )
    {
        SEGGER_RTT_WriteString(0, "BLE COM: data len too shortv");
        return;
    }
    ble_cmd = (ble_cmd_t)(data[0]);
    SEGGER_RTT_WriteString(0, "Recived new BLE");
    SEGGER_RTT_printf(0, "BLE CMD: %02X\r\n", data[0]);
    SEGGER_RTT_printf(0, "BLE PAR: %02X\r\n", data[1]);

    for ( int i = 0 ; i < (len - 2) ; i ++ )
    {
        SEGGER_RTT_printf(0, "BLE DAT[%d]: %02X\r\n",i, data[2+i]); 
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
        case COMMAND_CONTROLL:
            SEGGER_RTT_WriteString(0, "BLE COM: CMD=SET_CONTROLL\r\n");
            BLE_COM_parse_parameter_controll(&data[1], len - 1);
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
        case PARAM_SET_MIN_PWM:
            SEGGER_RTT_WriteString(0, "BLE COM: PARAM=SET_MIN_PWM\r\n");
            uint16_t min_pwm;
            min_pwm = ( (uint16_t)data[1] << 8 ) | data[2];
            SEGGER_RTT_printf(0, "BLE COM: SET_MIN_PWM=%d\r\n", min_pwm);
            printf("BLE COM: SET_MIN_PWM=%d\r\n", min_pwm);
            settings.min_pwm_duty = min_pwm;
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

            if ( effect_number >= TOTAL_EFFECT_NUMBER )
            {
                SEGGER_RTT_WriteString(0, "BLE COM: Wrong effect number\r\n");
                printf("BLE COM: Wrong effect number\r\n");
                return;
            }

            settings.selected_effect_number = effect_number;
            SEGGER_RTT_printf(0, "BLE COM: SET_EFFECT=%d\r\n", effect_number);
            printf("BLE COM: SET_EFFECT=%d\r\n", effect_number);

            settings.effect_settings_1[settings.selected_effect_number] = ( (uint16_t)data[2] << 8 ) | data[3];
            settings.effect_settings_2[settings.selected_effect_number] = ( (uint16_t)data[4] << 8 ) | data[5];
            settings.effect_settings_3[settings.selected_effect_number] = ( (uint16_t)data[6] << 8 ) | data[7];

            EFFECT_set_effect_settings();
        break; 
        case PARAM_SET_ALL_PARAM:
            SEGGER_RTT_WriteString(0, "BLE COM: PARAM=SET_ALL_PARAM\r\n");
            printf("BLE COM: PARAM=SET_ALL_PARAM\r\n");
            settings.num_of_stairs = data[1];
            settings.max_pwm_duty = ( (uint16_t)data[3] << 8 ) | data[2];
            settings.min_pwm_duty = ( (uint16_t)data[5] << 8 ) | data[4];
            settings.stair_light_on_time_ms = ( (uint16_t)data[9] << 8 ) | ( (uint16_t)data[8] << 8 ) | ( (uint16_t)data[7] << 8 ) | data[6];
            settings.selected_effect_number = data[10];
            settings.effect_settings_1[settings.selected_effect_number] = ( (uint16_t)data[12] << 8 ) | data[11];
            settings.effect_settings_2[settings.selected_effect_number] = ( (uint16_t)data[14] << 8 ) | data[13];
            settings.effect_settings_3[settings.selected_effect_number] = ( (uint16_t)data[16] << 8 ) | data[15];

            if ( settings.selected_effect_number == EFFECT_NUM_1 )
            {
                EFFECT_set_effect_1();
            }
            else if ( settings.selected_effect_number == EFFECT_NUM_2 )
            {
                EFFECT_set_effect_2();
            }
            else if ( settings.selected_effect_number == EFFECT_NUM_3 )
            {
                EFFECT_set_effect_3();
            }
            else
            {
                settings.selected_effect_number = SETTINGS_DEFAULT_SELECTED_EFFECT_NUMBER;
                EFFECT_set_effect_1();
            }

            EFFECT_set_effect_settings();

            SEGGER_RTT_printf(0, "BLE COM: SET_ALL_PARAM: num_of_stairs=%d\r\n", settings.num_of_stairs);
            SEGGER_RTT_printf(0, "BLE COM: SET_ALL_PARAM: max_pwm_duty=%d\r\n", settings.max_pwm_duty);
            SEGGER_RTT_printf(0, "BLE COM: SET_ALL_PARAM: min_pwm_duty=%d\r\n", settings.min_pwm_duty);
            SEGGER_RTT_printf(0, "BLE COM: SET_ALL_PARAM: stair_light_on_time_ms=%d\r\n", settings.stair_light_on_time_ms);
            SEGGER_RTT_printf(0, "BLE COM: SET_ALL_PARAM: selected_effect_number=%d\r\n", settings.selected_effect_number);
            SEGGER_RTT_printf(0, "BLE COM: SET_ALL_PARAM: effect1_settings_1=%d\r\n", settings.effect_settings_1[0]);
            SEGGER_RTT_printf(0, "BLE COM: SET_ALL_PARAM: effect1_settings_2=%d\r\n", settings.effect_settings_2[0]);
            SEGGER_RTT_printf(0, "BLE COM: SET_ALL_PARAM: effect1_settings_3=%d\r\n", settings.effect_settings_3[0]);
            SEGGER_RTT_printf(0, "BLE COM: SET_ALL_PARAM: effect2_settings_1=%d\r\n", settings.effect_settings_1[1]);
            SEGGER_RTT_printf(0, "BLE COM: SET_ALL_PARAM: effect2_settings_2=%d\r\n", settings.effect_settings_2[1]);
            SEGGER_RTT_printf(0, "BLE COM: SET_ALL_PARAM: effect2_settings_3=%d\r\n", settings.effect_settings_3[1]);
            SEGGER_RTT_printf(0, "BLE COM: SET_ALL_PARAM: effect3_settings_1=%d\r\n", settings.effect_settings_1[2]);
            SEGGER_RTT_printf(0, "BLE COM: SET_ALL_PARAM: effect3_settings_2=%d\r\n", settings.effect_settings_2[2]);
            SEGGER_RTT_printf(0, "BLE COM: SET_ALL_PARAM: effect3_settings_3=%d\r\n", settings.effect_settings_3[2]);

            save_flag = 1;
        break; 
        case PARAM_SET_READ_ALL:
            SEGGER_RTT_WriteString(0, "BLE COM: PARAM=PARAM_SET_READ_ALL\r\n");
            printf("BLE COM: PARAM=PARAM_SET_READ_ALL\r\n");
            SETTINGS_print_rtt();
            SETTINGS_print_serial();

            uint8_t data_to_send[23];

            data_to_send[0] = COMMAND_SET_PARAMETER;
            data_to_send[1] = PARAM_SET_READ_ALL;
            data_to_send[2] = settings.num_of_stairs;
            data_to_send[3] = (uint8_t)(settings.max_pwm_duty >> 8);
            data_to_send[4] = (uint8_t)(settings.max_pwm_duty & 0xFF);
            data_to_send[5] = (uint8_t)(settings.min_pwm_duty >> 8);
            data_to_send[6] = (uint8_t)(settings.min_pwm_duty & 0xFF);
            data_to_send[7] = (uint8_t)(settings.stair_light_on_time_ms >> 24);
            data_to_send[8] = (uint8_t)(settings.stair_light_on_time_ms >> 16);
            data_to_send[9] = (uint8_t)(settings.stair_light_on_time_ms >> 8);
            data_to_send[10] = (uint8_t)(settings.stair_light_on_time_ms & 0xFF);
            data_to_send[11] = FW_VERSION;
            data_to_send[12] = (uint8_t)( settings.reset_count >> 24 );
            data_to_send[13] = (uint8_t)( settings.reset_count >> 16 );
            data_to_send[14] = (uint8_t)( settings.reset_count >> 8 );
            data_to_send[15] = (uint8_t)( settings.reset_count & 0xFF );

            data_to_send[16] = settings.selected_effect_number;
            data_to_send[17] = (uint8_t)(settings.effect_settings_1[settings.selected_effect_number] >> 8);
            data_to_send[18] = (uint8_t)(settings.effect_settings_1[settings.selected_effect_number] & 0xFF);
            data_to_send[19] = (uint8_t)(settings.effect_settings_2[settings.selected_effect_number] >> 8);
            data_to_send[20] = (uint8_t)(settings.effect_settings_2[settings.selected_effect_number] & 0xFF);
            data_to_send[21] = (uint8_t)(settings.effect_settings_3[settings.selected_effect_number] >> 8);
            data_to_send[22] = (uint8_t)(settings.effect_settings_3[settings.selected_effect_number] & 0xFF);

            BLE_send(data_to_send, 23);
        break;
        case PARAM_SET_LOAD_AND_SAVE_DEFAULT:
            SEGGER_RTT_WriteString(0, "BLE COM: PARAM=PARAM_SET_LOAD_AND_SAVE_DEFAULT\r\n");
            printf("BLE COM: PARAM=PARAM_SET_LOAD_AND_SAVE_DEFAULT\r\n");
            SETTINGS_load_and_save_default();
            SETTINGS_print_rtt();
            SETTINGS_print_serial();
            save_flag = 1;
        break;
        case PARAM_SET_SAVE_ALL:
            SEGGER_RTT_WriteString(0, "BLE COM: PARAM=PARAM_SAVEL_ALL\r\n");
            printf("BLE COM: PARAM=PARAM_SAVEL_ALL\r\n");
            SETTINGS_print_rtt();
            SETTINGS_print_serial();
            save_flag = 1;
        break;
        case PARAM_ACT_SOFT_RESET:
            SEGGER_RTT_WriteString(0, "BLE COM: PARAM=PARAM_ACT_SOFT_RESET\r\n");
            EFFECT_stop_effect();  
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
            //printf("BLE COM: PARAM=ACT_PRESS_SW_1\r\n");
        break; 
        case PARAM_ACT_PRESS_SW_2:
            SEGGER_RTT_WriteString(0, "BLE COM: PARAM=ACT_PRESS_SW_2\r\n");
            //printf("BLE COM: PARAM=ACT_PRESS_SW_2\r\n");
        break;
        case PARAM_ACT_PRESS_TOP_PRESS:
            SEGGER_RTT_WriteString(0, "BLE COM: PARAM=ACT_PRESS_TOP_PRESS\r\n");
            //printf("BLE COM: PARAM=ACT_PRESS_TOP_PRESS\r\n");
            action_sensor_top();
        break; 
        case PARAM_ACT_PRESS_BOTTOM_PRESS:
            SEGGER_RTT_WriteString(0, "BLE COM: PARAM=ACT_PRESS_BOTTOM_PRESS\r\n");
            //printf("BLE COM: PARAM=ACT_PRESS_BOTTOM_PRESS\r\n");
            action_sensor_bottom();
        break; 
        case PARAM_ACT_SOFT_RESET:
            SEGGER_RTT_WriteString(0, "BLE COM: PARAM=ACT_SOFT_RESET\r\n");
            //printf("BLE COM: PARAM=ACT_SOFT_RESET\r\n");
            EFFECT_stop_effect();
            PWM_set_all(0);
        default:
            SEGGER_RTT_WriteString(0, "BLE COM: PARAM=ACT Unknow\r\n");
            //printf("BLE COM: PARAM=ACT Unknow\r\n");
        break;
    }    
}


static void BLE_COM_parse_parameter_controll(uint8_t* data, uint16_t len)
{
    uint16_t manual_brightness;

    ble_param = (ble_cmd_t)(data[0]);

    switch(ble_param)
    {
        case PARAM_CTRL_MANUAL_BRIGHTNESS_CONTROL_ENABLE_DISABLE:
            manual_brightness_control_flag = data[1];
            SEGGER_RTT_printf(0, "BLE COM: PARAM=ACT_MANUAL_BRIGHTNESS_CONTROL_ENABLE_DISABLE: %d\r\n", manual_brightness_control_flag);
            if ( manual_brightness_control_flag == 0 )
            {
                PWM_set_all( 0 );
            }
        break;
        case PARAM_CTRL_SET_MANUAL_BRIGHTNESS:
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


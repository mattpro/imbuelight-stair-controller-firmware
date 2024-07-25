/**
 * Copyright (c) 2023 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include "btstack.h"
#include "server_common.h"
#include "temp_sensor.h"
#include "ble_communication.h"
#include "effects.h"
#include "settings.h"
#include "config.h"

#include "rtt/RTT/SEGGER_RTT.h"

#define APP_AD_FLAGS 0x06
static uint8_t adv_data[] = {
    // Flags general discoverable
    0x02, BLUETOOTH_DATA_TYPE_FLAGS, APP_AD_FLAGS,
    // Name
    0x19, BLUETOOTH_DATA_TYPE_COMPLETE_LOCAL_NAME, 'I', 'L', ' ', 'S', 't', 'a', 'i', 'r', ' ', 'a', 'b', 'c', 'd', 'e', 'f', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',' '
};
static const uint8_t adv_data_len = sizeof(adv_data);

int le_notification_enabled;
hci_con_handle_t con_handle;
uint16_t current_temp;

uint8_t connected_flag = 0;

static uint8_t ble_send_buffer[100];
static uint16_t ble_send_buffer_len = 0;

void BLE_send(uint8_t *data, uint16_t len)
{
    memcpy(ble_send_buffer, data, len);
    ble_send_buffer_len = len;
    if (le_notification_enabled) 
    {
        att_server_request_can_send_now_event(con_handle);
    }
}


void packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size) 
{
    UNUSED(size);
    UNUSED(channel);
    bd_addr_t local_addr;
    if (packet_type != HCI_EVENT_PACKET) 
    {
        return;
    }

    uint8_t event_type = hci_event_packet_get_type(packet);

    //SEGGER_RTT_printf(0, "BLE event: %02X\n", event_type);

    switch(event_type)
    {        
        case ATT_EVENT_CONNECTED:
            SEGGER_RTT_WriteString(0, "BLE Connected\n");
            connected_flag = 1;
        break;
        case ATT_EVENT_DISCONNECTED:
            SEGGER_RTT_WriteString(0, "BLE Disconnected\n");
        break;
        case BTSTACK_EVENT_STATE:
            if (btstack_event_state_get_state(packet) != HCI_STATE_WORKING) return;
            gap_local_bd_addr(local_addr);
            SEGGER_RTT_printf(0, "BTstack up and running on %s.\n", bd_addr_to_str(local_addr));

            // Generate device advertising name
            uint8_t addr_s[3];
            addr_s[0] = local_addr[0] ^ local_addr[1];
            addr_s[1] = local_addr[2] ^ local_addr[3];
            addr_s[2] = local_addr[4] ^ local_addr[5];
            sprintf(&adv_data[5], "IL Stair %02X%02X%02X", addr_s[0], addr_s[1], addr_s[2]);

            // setup advertisements
            uint16_t adv_int_min = 800;
            uint16_t adv_int_max = 800;
            uint8_t adv_type = 0;
            bd_addr_t null_addr;
            memset(null_addr, 0, 6);
            gap_advertisements_set_params(adv_int_min, adv_int_max, adv_type, 0, null_addr, 0x07, 0x00);
            assert(adv_data_len <= 31); // ble limitation
            gap_advertisements_set_data(adv_data_len, (uint8_t*) adv_data);
            gap_advertisements_enable(1);
        break;
        case HCI_EVENT_DISCONNECTION_COMPLETE:
            le_notification_enabled = 0;
        break;
        case ATT_EVENT_CAN_SEND_NOW:
            SEGGER_RTT_WriteString(0, "BLE ATT_EVENT_CAN_SEND_NOW\n");
            //current_temp ++;
            //att_server_notify(con_handle, ATT_CHARACTERISTIC_6E400003_B5A3_F393_E0A9_E50E24DCCA9E_01_VALUE_HANDLE, (uint8_t*)&current_temp, sizeof(current_temp));
            att_server_notify(con_handle, ATT_CHARACTERISTIC_6E400003_B5A3_F393_E0A9_E50E24DCCA9E_01_VALUE_HANDLE, ble_send_buffer, ble_send_buffer_len); 
        break;
        default:
        break;
    }
}

uint16_t att_read_callback(hci_con_handle_t connection_handle, uint16_t att_handle, uint16_t offset, uint8_t * buffer, uint16_t buffer_size) 
{
    UNUSED(connection_handle);

    SEGGER_RTT_printf(0, "BLE read. Len=%d data=%s", buffer_size, buffer);

    uint8_t data_to_send[23];
    uint32_t data_pointer = 0;
    
    memcpy((void*)&data_to_send[0], (void*)&settings.reset_count, sizeof(uint32_t) );
    data_pointer += sizeof(uint32_t);

    memcpy((void*)&data_to_send[data_pointer], (void*)&settings.num_of_stairs, sizeof(uint8_t) );
    data_pointer += sizeof(uint8_t);
    
    memcpy((void*)&data_to_send[data_pointer], (void*)&settings.max_pwm_duty, sizeof(uint16_t) );
    data_pointer += sizeof(uint16_t);
    
    memcpy((void*)&data_to_send[data_pointer], (void*)&settings.min_pwm_duty, sizeof(uint16_t) );
    data_pointer += sizeof(uint16_t);

    memcpy((void*)&data_to_send[data_pointer], (void*)&settings.stair_light_on_time_ms, sizeof(uint32_t) );
    data_pointer += sizeof(uint32_t);

    memcpy((void*)&data_to_send[data_pointer], (void*)&settings.selected_effect_number, sizeof(uint8_t) );
    data_pointer += sizeof(uint8_t);

    memcpy((void*)&data_to_send[data_pointer], (void*)&settings.effect_settings_1[settings.selected_effect_number], sizeof(uint16_t) );
    data_pointer += sizeof(uint16_t);

    memcpy((void*)&data_to_send[data_pointer], (void*)&settings.effect_settings_2[settings.selected_effect_number], sizeof(uint16_t) );
    data_pointer += sizeof(uint16_t);

    memcpy((void*)&data_to_send[data_pointer], (void*)&settings.effect_settings_3[settings.selected_effect_number], sizeof(uint16_t) );
    data_pointer += sizeof(uint16_t);

    memcpy((void*)&data_to_send[data_pointer], (void*)&light_on.light_on_flag, sizeof(bool) );
    data_pointer += sizeof(bool);

    memcpy((void*)&data_to_send[data_pointer], (void*)&settings.reset_count, sizeof(bool) );
    data_pointer += sizeof(bool);

    uint8_t fw_version = FW_VERSION;
    memcpy((void*)&data_to_send[data_pointer], (void*)&fw_version, sizeof(uint8_t) );
    data_pointer += sizeof(uint8_t);
                        
    if (att_handle == ATT_CHARACTERISTIC_6E400003_B5A3_F393_E0A9_E50E24DCCA9E_01_VALUE_HANDLE){
        return att_read_callback_handle_blob((const uint8_t *)&data_to_send, data_pointer, offset, buffer, buffer_size);
    }
    return 0;
}

int att_write_callback(hci_con_handle_t connection_handle, uint16_t att_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size) 
{
    UNUSED(transaction_mode);
    UNUSED(offset);
    UNUSED(buffer_size);
    
    SEGGER_RTT_printf(0, "BLE write. Len=%d data=%s", buffer_size, buffer);

    
    BLE_COM_parse(buffer, buffer_size);

    if (att_handle != ATT_CHARACTERISTIC_6E400003_B5A3_F393_E0A9_E50E24DCCA9E_01_CLIENT_CONFIGURATION_HANDLE) return 0;

    le_notification_enabled = little_endian_read_16(buffer, 0) == GATT_CLIENT_CHARACTERISTICS_CONFIGURATION_NOTIFICATION;
    con_handle = connection_handle;
    if (le_notification_enabled) {
        att_server_request_can_send_now_event(con_handle);
    }
    return 0;
}

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "config.h"
#include "hardware/flash.h"
#include "my_flash.h"
#include "pwm.h"
#include "effects.h"
#include "rtt/RTT/SEGGER_RTT.h"
#include "pico/stdlib.h"

#define FLASH_TARGET_OFFSET (512 * 1024)

const uint8_t *flash_target_contents = (const uint8_t *) (XIP_BASE + FLASH_TARGET_OFFSET);

volatile settings_t settings;


void SETTINGS_load(void)
{
    uint32_t data_pointer = 0;

    memcpy((void*)&settings.reset_count, (void*)&flash_target_contents[0], sizeof(uint32_t) );
    data_pointer += sizeof(uint32_t);

    if ( settings.reset_count == 0xFFFFFFFF )
    {
        settings.reset_count = 0;
    }

    memcpy((void*)&settings.num_of_stairs, (void*)&flash_target_contents[data_pointer], sizeof(uint8_t) );
    data_pointer += sizeof(uint8_t);
    if ( settings.num_of_stairs > MAX_NUM_STAIR )
    {
        settings.num_of_stairs = SETTINGS_DEFAULT_NUM_OF_STAIRS;
    }


    memcpy((void*)&settings.max_pwm_duty, (void*)&flash_target_contents[data_pointer], sizeof(uint16_t) );
    data_pointer += sizeof(uint16_t);

    memcpy((void*)&settings.stair_light_on_time_ms, (void*)&flash_target_contents[data_pointer], sizeof(uint32_t) );
    data_pointer += sizeof(uint32_t);

    memcpy((void*)&settings.selected_effect_number, (void*)&flash_target_contents[data_pointer], sizeof(uint8_t) );
    data_pointer += sizeof(uint8_t);

    memcpy((void*)&settings.effect1_settings_1, (void*)&flash_target_contents[data_pointer], sizeof(uint16_t) );
    data_pointer += sizeof(uint16_t);

    memcpy((void*)&settings.effect1_settings_2, (void*)&flash_target_contents[data_pointer], sizeof(uint16_t) );
    data_pointer += sizeof(uint16_t);

    memcpy((void*)&settings.effect2_settings_1, (void*)&flash_target_contents[data_pointer], sizeof(uint16_t) );
    data_pointer += sizeof(uint16_t);

    effect_1.increment_step = settings.effect1_settings_1;
    effect_1.wide = settings.effect1_settings_2;
    effect_2.increment_step = settings.effect2_settings_1;


    SEGGER_RTT_printf(0, "Flash target contets: %X\r\n", flash_target_contents);
    SEGGER_RTT_printf(0, "LOAD SETTINGS FROM FLASH:\r\n");
    SEGGER_RTT_printf(0, "reset_count: %d\r\n", settings.reset_count);
    SEGGER_RTT_printf(0, "num_of_stairs: %d\r\n", settings.num_of_stairs);
    SEGGER_RTT_printf(0, "max_pwm_duty: %d\r\n", settings.max_pwm_duty);
    SEGGER_RTT_printf(0, "stair_light_on_time_ms: %d\r\n", settings.stair_light_on_time_ms);
    SEGGER_RTT_printf(0, "selected_effect_number: %d\r\n", settings.selected_effect_number);
    SEGGER_RTT_printf(0, "effect1_settings_1: %d\r\n", settings.effect1_settings_1);
    SEGGER_RTT_printf(0, "effect1_settings_2: %d\r\n", settings.effect1_settings_2);
    SEGGER_RTT_printf(0, "effect2_settings_1: %d\r\n", settings.effect2_settings_1);
}


void SETTINGS_save(void)
{
    uint8_t data_to_save[FLASH_PAGE_SIZE];
    uint32_t data_pointer = 0;
    
    memcpy((void*)&data_to_save[0], (void*)&settings.reset_count, sizeof(uint32_t) );
    data_pointer += sizeof(uint32_t);

    memcpy((void*)&data_to_save[data_pointer], (void*)&settings.num_of_stairs, sizeof(uint8_t) );
    data_pointer += sizeof(uint8_t);
    
    memcpy((void*)&data_to_save[data_pointer], (void*)&settings.max_pwm_duty, sizeof(uint16_t) );
    data_pointer += sizeof(uint16_t);
    
    memcpy((void*)&data_to_save[data_pointer], (void*)&settings.stair_light_on_time_ms, sizeof(uint32_t) );
    data_pointer += sizeof(uint32_t);

    memcpy((void*)&data_to_save[data_pointer], (void*)&settings.selected_effect_number, sizeof(uint8_t) );
    data_pointer += sizeof(uint8_t);

    memcpy((void*)&data_to_save[data_pointer], (void*)&settings.effect1_settings_1, sizeof(uint16_t) );
    data_pointer += sizeof(uint16_t);

    memcpy((void*)&data_to_save[data_pointer], (void*)&settings.effect1_settings_2, sizeof(uint16_t) );
    data_pointer += sizeof(uint16_t);

    memcpy((void*)&data_to_save[data_pointer], (void*)&settings.effect2_settings_1, sizeof(uint16_t) );
    data_pointer += sizeof(uint16_t);

    SEGGER_RTT_WriteString(0,"Erasing target region...\r\n");
    flash_range_erase(FLASH_TARGET_OFFSET, FLASH_SECTOR_SIZE);

    SEGGER_RTT_WriteString(0,"Programming target region...\r\n");
    flash_range_program(FLASH_TARGET_OFFSET, data_to_save, FLASH_PAGE_SIZE);

    SEGGER_RTT_printf(0, "SAVE TO FLASH\r\n");
}


void SETTINGS_load_and_save_default(void)
{
    settings.num_of_stairs = SETTINGS_DEFAULT_NUM_OF_STAIRS;
    settings.max_pwm_duty = SETTINGS_DEFAULT_MAX_PWM;
    settings.stair_light_on_time_ms = SETTINGS_DEFAULT_STAIR_LIGHT_ON_TIME_MS;
    settings.selected_effect_number = SETTINGS_DEFAULT_SELECTED_EFFECT_NUMBER;

    settings.effect1_settings_1 = SETTINGS_DEFAULT_EFFECT_1_SETTINGS_1;
    settings.effect1_settings_2 = SETTINGS_DEFAULT_EFFECT_1_SETTINGS_2;
    settings.effect2_settings_1 = SETTINGS_DEFAULT_EFFECT_2_SETTINGS_1;

    effect_1.increment_step = settings.effect1_settings_1;
    effect_1.wide = settings.effect1_settings_2;

    effect_2.increment_step = settings.effect2_settings_1;

    SETTINGS_save();
}


void SETTINGS_load_and_increase_reset_count(void)
{
    SETTINGS_load();
    settings.reset_count++;
    SETTINGS_save();
}


void SETTINGS_print_rtt(void)
{
    SEGGER_RTT_printf(0, "Flash target contets: %X\r\n", flash_target_contents);
    SEGGER_RTT_printf(0, "LOAD SETTINGS FROM FLASH:\r\n");
    SEGGER_RTT_printf(0, "reset_count: %d\r\n", settings.reset_count);
    SEGGER_RTT_printf(0, "num_of_stairs: %d\r\n", settings.num_of_stairs);
    SEGGER_RTT_printf(0, "max_pwm_duty: %d\r\n", settings.max_pwm_duty);
    SEGGER_RTT_printf(0, "stair_light_on_time_ms: %d\r\n", settings.stair_light_on_time_ms);
    SEGGER_RTT_printf(0, "selected_effect_number: %d\r\n", settings.selected_effect_number);
    SEGGER_RTT_printf(0, "effect1_settings_1: %d\r\n", settings.effect1_settings_1);
    SEGGER_RTT_printf(0, "effect1_settings_2: %d\r\n", settings.effect1_settings_2);
    SEGGER_RTT_printf(0, "effect2_settings_1: %d\r\n", settings.effect2_settings_1);
}


void SETTINGS_print_serial(void)
{
    printf("Flash target contets: %X\r\n", flash_target_contents);
    printf("LOAD SETTINGS FROM FLASH:\r\n");
    printf("reset_count: %d\r\n", settings.reset_count);
    printf("num_of_stairs: %d\r\n", settings.num_of_stairs);
    printf("max_pwm_duty: %d\r\n", settings.max_pwm_duty);
    printf("stair_light_on_time_ms: %d\r\n", settings.stair_light_on_time_ms);
    printf("selected_effect_number: %d\r\n", settings.selected_effect_number);
    printf("effect1_settings_1: %d\r\n", settings.effect1_settings_1);
    printf("effect1_settings_2: %d\r\n", settings.effect1_settings_2);
    printf("effect2_settings_1: %d\r\n", settings.effect2_settings_1);
}

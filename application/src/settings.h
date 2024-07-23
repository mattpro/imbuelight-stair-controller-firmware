#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#include <stdint.h>



typedef struct {    
    uint32_t reset_count;
    uint8_t num_of_stairs;
    uint16_t max_pwm_duty;
    uint16_t min_pwm_duty;
    uint32_t stair_light_on_time_ms;
    uint8_t selected_effect_number; 
    uint16_t effect1_settings_1;
    uint16_t effect1_settings_2;
    uint16_t effect2_settings_1;
} settings_t;


extern volatile settings_t settings;


void SETTINGS_load(void);
void SETTINGS_save(void);
void SETTINGS_load_and_save_default(void);
void SETTINGS_load_and_increase_reset_count(void);
void SETTINGS_print_rtt(void);
void SETTINGS_print_serial(void);

#endif

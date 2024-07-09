#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#include <stdint.h>


#define SETTINGS_DEFAULT_NUM_OF_STAIRS              23
#define SETTINGS_DEFAULT_MAX_PWM                    60000 //MAX_PWM_DUTY
#define SETTINGS_DEFAULT_STAIR_LIGHT_ON_TIME_MS     10000
#define SETTINGS_DEFAULT_SELECTED_EFFECT_NUMBER     1
#define SETTINGS_DEFAULT_EFFECT_1_SETTINGS_1        1        // Effect 1 and 2 increment_step
#define SETTINGS_DEFAULT_EFFECT_1_SETTINGS_2        2000        // Effect 2 wide 
#define SETTINGS_DEFAULT_EFFECT_2_SETTINGS_1        10


typedef struct {    
    uint32_t reset_count;
    uint8_t num_of_stairs;
    uint16_t max_pwm_duty;
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

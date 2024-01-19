#ifndef _MYFLASH_H_
#define _MYFLASH_H_

#include <stdint.h>

typedef struct {    
    uint32_t reset_count;
    uint8_t num_of_stairs;
    uint16_t max_pwm_duty;
    uint32_t stair_light_on_time_ms;
} settings_t;


extern volatile settings_t settings;


void SETTINGS_load(void);
void SETTINGS_save(void);
void SETTINGS_load_and_save_default(void);
void SETTINGS_increase_reset_count(void);


#endif

#ifndef _EFFECTS_H_
#define _EFFECTS_H_

#include <stdint.h>
#include "pico/stdlib.h"

#define EFFECT_NUM_1 1
#define EFFECT_NUM_2 2


#define EFFECT_DISABLE_AFTER_ON     1
#define EFFECT_DISABLE_AFTER_OFF    2
#define EFFECT_ON                   3
#define EFFECT_OFF                  4


typedef enum {
    DIR_DOWN_TO_UP,
    DIR_UP_TO_DOWN
} effect_dir_t;

typedef enum {
    TURN_ON,
    TURN_OFF
} turn_dir_t;


typedef struct {
    bool light_on_flag;
    bool light_on_from_top_flag;
    bool light_on_from_bottom_flag;
    uint32_t light_off_counter_ms;
    effect_dir_t dir;
} light_on_t;


typedef struct {
    int step;                   // current effect step
    bool enable;                // enable effect
    effect_dir_t dir;           // direction DIR_DOWN_TO_UP or DIR_UP_TO_DOWN    
    turn_dir_t turn_dir;        // if enable effect goes on or off
    // settings
    uint16_t increment_step;    // speed of effect
    uint16_t wide;              // wide of effect
} stair_effect_t;


typedef struct {
    void (*effect_start)(effect_dir_t dir); 
    void (*effect_end) (effect_dir_t dir); 
    int (*effect_loop) (void); 
} stair_effect_event_t;


extern volatile stair_effect_t effect_1;
extern volatile stair_effect_t effect_2;
extern volatile light_on_t light_on;
extern stair_effect_event_t stair_effect_event;


void EFFECT_set_effect_1(void);
void EFFECT_set_effect_2(void);


#endif

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "effects.h"
#include "my_flash.h"
#include "config.h"
#include "pwm.h"
#include "pwm_pio.h"
#include "pico/stdlib.h"



volatile light_on_t light_on;
volatile stair_effect_t effect_1;
volatile stair_effect_t effect_2;




static void effect_1_start(effect_dir_t dir);
static void effect_1_end(effect_dir_t dir);
static void effect_1_loop(void);

static void effect_2_start(effect_dir_t dir);
static void effect_2_end(effect_dir_t dir);
static void effect_2_loop(void);



stair_effect_event_t stair_effect_event =
{
    effect_2_start,
    effect_2_end,
    effect_2_loop,
};


void EFFECT_set_effect_1(void)
{
    stair_effect_event.effect_start = effect_1_start;
    stair_effect_event.effect_end = effect_1_end;
    stair_effect_event.effect_loop = effect_1_loop;
    effect_1.enable = true;
    settings.selected_effect_number = 1;
}

void EFFECT_set_effect_2(void)
{
    stair_effect_event.effect_start = effect_2_start;
    stair_effect_event.effect_end = effect_2_end;
    stair_effect_event.effect_loop = effect_2_loop;
    effect_2.enable = true;
    settings.selected_effect_number = 2;
}




// ###########################################
// ################ EFFECT 1  ################
// ###########################################

static void effect_1_start(effect_dir_t dir)
{
    effect_1.dir = dir;
    effect_1.step = 0;
    effect_1.turn_dir = TURN_ON;
    effect_1.enable = true;
}

static void effect_1_end(effect_dir_t dir)
{
    effect_1.dir = dir;
    effect_1.step = 0;
    effect_1.turn_dir = TURN_OFF;
    effect_1.enable = true;
}


static void effect_1_loop(void)
{
    if ( effect_1.enable )
    {
        uint8_t step_number;
        int calculate_duty;

        effect_1.step += effect_1.increment_step; 

        SEGGER_RTT_WriteString(0,"------------------------------\r\n");

        // Update each step PWM
        for ( int i = 0 ; i < settings.num_of_stairs ; i ++ )
        {
            if      ( effect_1.dir == DIR_UP_TO_DOWN )  step_number = i;
            else if ( effect_1.dir == DIR_DOWN_TO_UP )  step_number = settings.num_of_stairs - i - 1;
            
            if      ( effect_1.turn_dir == TURN_ON  )   calculate_duty = -effect_1.wide*i + effect_1.step;
            else if ( effect_1.turn_dir == TURN_OFF )   calculate_duty = settings.max_pwm_duty + effect_1.wide*i - effect_1.step;

            PWM_set_duty_in_channel_with_gamma_max_duty((pwm_channel_t)step_number, calculate_duty, settings.max_pwm_duty);

            SEGGER_RTT_printf(0, "[%2d]=%d\r\n", i, calculate_duty);
        }

        // Check ending condition
        if ( ( effect_1.turn_dir == TURN_ON  ) && ( calculate_duty > settings.max_pwm_duty ) )
        {
            SEGGER_RTT_WriteString(0,"Effect animation disable after turn on\r\n");
            effect_1.enable = false;
        }
        else if ( ( effect_1.turn_dir == TURN_OFF ) && ( calculate_duty < 0 ) )
        {
            SEGGER_RTT_WriteString(0,"Effect animation disable after turn off\r\n");
            effect_1.enable = false;
            light_on.light_on_from_top_flag = false; 
            light_on.light_on_from_bottom_flag = false; 
        }
    }
}

// ###########################################
// ################ EFFECT 2  ################
// ###########################################

static void effect_2_start(effect_dir_t dir)
{
    effect_2.step = 0;
    effect_2.turn_dir = TURN_ON;
    effect_2.enable = true;
}

static void effect_2_end(effect_dir_t dir)
{
    effect_2.step = 0;
    effect_2.turn_dir = TURN_OFF;
    effect_2.enable = true;
}


static void effect_2_loop(void)
{
    if ( effect_2.enable )
    {
        effect_2.step += effect_2.increment_step;

        for ( int i = 0 ; i < settings.num_of_stairs ; i ++ )
        {
            if ( effect_2.turn_dir == TURN_ON )
            {
                PWM_set_duty_in_channel_with_gamma((pwm_channel_t)i,  effect_2.step );
            }
            else if ( effect_2.turn_dir == TURN_OFF )
            {
                PWM_set_duty_in_channel_with_gamma((pwm_channel_t)i,  settings.max_pwm_duty - effect_2.step );
            }
        }

        // End effect condition
        if ( ( effect_2.turn_dir == TURN_ON ) && (  effect_2.step > settings.max_pwm_duty ) )
        {
            effect_2.enable = false;
        }
        if ( ( effect_2.turn_dir == TURN_OFF ) && (  effect_2.step < 0 ) )
        {
            effect_2.enable = false;
        }
    }
}



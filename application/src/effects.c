#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "effects.h"
#include "settings.h"
#include "config.h"
#include "pwm.h"
#include "pwm_pio.h"
#include "pico/stdlib.h"



volatile light_on_t light_on;
volatile stair_effect_t effect_1;
volatile stair_effect_t effect_2;
volatile stair_effect_t effect_3;
volatile stair_effect_t effect_4;

static int steps_calc_duty[23] = {0};
static int steps_real_duty[23] = {0};

static void effect_1_start(effect_dir_t dir);
static void effect_1_end(effect_dir_t dir);
static int effect_1_loop(void);

static void effect_2_start(effect_dir_t dir);
static void effect_2_end(effect_dir_t dir);
static int effect_2_loop(void);

static void effect_3_start(effect_dir_t dir);
static void effect_3_end(effect_dir_t dir);
static int effect_3_loop(void);

// static void effect_4_start(effect_dir_t dir);
// static void effect_4_end(effect_dir_t dir);
// static int effect_4_loop(void);


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
    settings.selected_effect_number = 0;
}

void EFFECT_set_effect_2(void)
{
    stair_effect_event.effect_start = effect_2_start;
    stair_effect_event.effect_end = effect_2_end;
    stair_effect_event.effect_loop = effect_2_loop;
    settings.selected_effect_number = 1;
}

void EFFECT_set_effect_3(void)
{
    stair_effect_event.effect_start = effect_3_start;
    stair_effect_event.effect_end = effect_3_end;
    stair_effect_event.effect_loop = effect_3_loop;
    settings.selected_effect_number = 2;
}

// void EFFECT_set_effect_4(void)
// {
//     stair_effect_event.effect_start = effect_4_start;
//     stair_effect_event.effect_end = effect_4_end;
//     stair_effect_event.effect_loop = effect_4_loop;
//     settings.selected_effect_number = 4;
// }



void EFFECT_stop_effect(void)
{
    effect_1.enable = false;
    effect_2.enable = false;
    effect_3.enable = false;

    effect_1.step = 0;
    effect_2.step = 0;
    effect_3.step = 0;

    light_on.light_off_counter_ms = 0;
    light_on.light_on_from_top_flag = false;
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




static int effect_1_loop(void)
{
    if ( effect_1.enable )
    {
        uint8_t step_number;
        int calculate_duty;
        int duty_set;

        effect_1.step += effect_1.increment_step; 

        SEGGER_RTT_printf(0,"%6d ------------------------------\r\n", effect_1.step );

        // Update each step PWM
        for ( int i = 0 ; i < settings.num_of_stairs ; i ++ )
        {
            if      ( effect_1.dir == DIR_UP_TO_DOWN )  step_number = i;
            else if ( effect_1.dir == DIR_DOWN_TO_UP )  step_number = settings.num_of_stairs - i - 1;
            
            if      ( effect_1.turn_dir == TURN_ON  )   calculate_duty = -effect_1.wide*i + effect_1.step + settings.min_pwm_duty;
            else if ( effect_1.turn_dir == TURN_OFF )   calculate_duty = settings.max_pwm_duty - ( -effect_1.wide*i + effect_1.step );

            duty_set = PWM_set_duty_in_channel_with_gamma_max_duty((pwm_channel_t)step_number, calculate_duty, settings.max_pwm_duty);

            steps_real_duty[i] = duty_set;
            steps_calc_duty[i] = calculate_duty;

            SEGGER_RTT_printf(0, "[%2d]=%6d || %6d\r\n", i, calculate_duty, duty_set);
        }

        // Check ending condition
        if ( ( effect_1.turn_dir == TURN_ON  ) && ( calculate_duty >= settings.max_pwm_duty ) )
        {
            SEGGER_RTT_WriteString(0,"Effect animation disable after turn on\r\n");
            effect_1.enable = false;   
            
            return EFFECT_DISABLE_AFTER_ON;
        }
        else if ( ( effect_1.turn_dir == TURN_OFF ) && ( calculate_duty < 0 ) )
        {
            SEGGER_RTT_WriteString(0,"Effect animation disable after turn off\r\n");
            effect_1.enable = false;
            light_on.light_on_from_top_flag = false; 
            light_on.light_on_from_bottom_flag = false; 

            return EFFECT_DISABLE_AFTER_OFF;
        }

        return EFFECT_ON;
    }
    return EFFECT_OFF;
}

// ###########################################
// ################ EFFECT 2  ################
// ###########################################

static void effect_2_start(effect_dir_t dir)
{
    effect_2.dir = dir;
    effect_2.step = 0;
    effect_2.turn_dir = TURN_ON;
    effect_2.enable = true;
}

static void effect_2_end(effect_dir_t dir)
{
    effect_2.dir = dir;
    effect_2.step = 0;
    effect_2.turn_dir = TURN_OFF;
    effect_2.enable = true;
}


static int effect_2_loop(void)
{
    int calculate_duty;
    int duty_set;

    if ( effect_2.enable )
    {
        effect_2.step += effect_2.increment_step;

        for ( int step_num = 0 ; step_num < settings.num_of_stairs ; step_num ++ )
        {
            if ( effect_2.turn_dir == TURN_ON )
            {
                calculate_duty = effect_2.step;   
            }
            else if ( effect_2.turn_dir == TURN_OFF )
            {
                calculate_duty = settings.max_pwm_duty - effect_2.step;
            }
            duty_set = PWM_set_duty_in_channel_with_gamma_max_duty((pwm_channel_t)step_num, calculate_duty, settings.max_pwm_duty );
        }
    
        SEGGER_RTT_printf(0, "[%2d]=%6d || %6d\r\n", calculate_duty, duty_set);

        // End effect condition
        if ( ( effect_2.turn_dir == TURN_ON ) && (  calculate_duty > settings.max_pwm_duty ) )
        {
            SEGGER_RTT_WriteString(0,"Effect animation disable after turn on\r\n");
            effect_2.enable = false;

            return EFFECT_DISABLE_AFTER_ON;
        }
        if ( ( effect_2.turn_dir == TURN_OFF ) && (  calculate_duty < 0 ) )
        {
            SEGGER_RTT_WriteString(0,"Effect animation disable after turn off\r\n");
            effect_2.enable = false;

            return EFFECT_DISABLE_AFTER_OFF;
        }

        return EFFECT_ON;
    }
    
    return EFFECT_OFF;
}


// ###########################################
// ################ EFFECT 3  ################
// ###########################################

static void effect_3_start(effect_dir_t dir)
{
    effect_3.step = 0;
    effect_3.turn_dir = TURN_ON;
    effect_3.enable = true;
}

static void effect_3_end(effect_dir_t dir)
{
    effect_3.step = 0;
    effect_3.turn_dir = TURN_OFF;
    effect_3.enable = true;
}


static int effect_3_loop(void)
{
    if ( effect_3.enable )
    {
        int calculate_duty;
        int duty_set;
        int random_duty;
        static int delay_counter = 0;

        if ( delay_counter < effect_3.delay )
        {
            delay_counter += MAIN_TIMER_LOOP_MS;
            return EFFECT_ON;
        }
        else
        {
            delay_counter = 0;
        }
        effect_3.step += effect_3.increment_step; 


        SEGGER_RTT_printf(0,"%6d ------------------------------\r\n", effect_3.step );

        // Update each step PWM
        for ( int step_num = 0 ; step_num < settings.num_of_stairs ; step_num ++ )
        {
            random_duty = rand() % effect_3.random_width;

            if      ( effect_3.turn_dir == TURN_ON  )   calculate_duty = effect_3.step + settings.min_pwm_duty + random_duty;
            else if ( effect_3.turn_dir == TURN_OFF )   calculate_duty = settings.max_pwm_duty - ( effect_3.step + random_duty);

            duty_set = PWM_set_duty_in_channel_with_gamma_max_duty((pwm_channel_t)step_num, calculate_duty, settings.max_pwm_duty);

            steps_calc_duty[step_num] = calculate_duty;
            steps_real_duty[step_num] = duty_set;

            SEGGER_RTT_printf(0, "[%2d]=%6d || %6d\r\n", step_num, calculate_duty, duty_set);
        }

        // Check ending condition

        bool turn_on_end_condition = true;
        bool turn_off_end_condition = true;
        for ( int step_num = 0 ; step_num < settings.num_of_stairs ; step_num ++ )
        {
            if ( effect_3.turn_dir == TURN_ON )
            {
                if ( steps_calc_duty[step_num] < settings.max_pwm_duty )
                {
                    turn_on_end_condition = false;
                    break;
                }
            }
            else if ( effect_3.turn_dir == TURN_OFF )
            {
                if ( steps_calc_duty[step_num] > 0 )
                {
                    turn_off_end_condition = false;
                    break;
                }
            }
        }

        if ( ( effect_3.turn_dir == TURN_ON  ) && ( turn_on_end_condition ) )
        {
            SEGGER_RTT_WriteString(0,"Effect animation disable after turn on\r\n");
            effect_3.enable = false;   
            
            return EFFECT_DISABLE_AFTER_ON;
        }
        else if ( ( effect_3.turn_dir == TURN_OFF ) && ( turn_off_end_condition ) )
        {
            SEGGER_RTT_WriteString(0,"Effect animation disable after turn off\r\n");
            effect_3.enable = false;
            light_on.light_on_from_top_flag = false; 
            light_on.light_on_from_bottom_flag = false; 

            return EFFECT_DISABLE_AFTER_OFF;
        }

        return EFFECT_ON;
    }
    return EFFECT_OFF;
}


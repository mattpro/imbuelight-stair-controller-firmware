#include <stdio.h>
#include "pico/stdlib.h"
#include "utils.h"
#include "config.h"


void delay_boot_info(void)
{
    printf("############################################\r\n");
    printf("### IMBUE LIGHT STAIR CONTROLLER VER %s ###\r\n", FW_VERSION_STR);
    printf( "###         %s %s         ###\r\n", __DATE__, __TIME__);
    printf("############################################\r\n");
}


void boot_info_rtt(void)
{
    SEGGER_RTT_WriteString(0,"############################################\r\n");
    SEGGER_RTT_printf(0,"### IMBUE LIGHT STAIR CONTROLLER VER %s ###\r\n", FW_VERSION_STR);
    SEGGER_RTT_printf(0, "###         %s %s         ###\r\n", __DATE__, __TIME__);
    SEGGER_RTT_WriteString(0,"############################################\r\n");
}


void start_blink_led(void)
{
    gpio_init(PWM_CHANNEL_1_PIN);
    gpio_set_dir(PWM_CHANNEL_1_PIN, GPIO_OUT);
    for( int i = 0 ; i < 20 ; i ++ )
    {
        //SEGGER_RTT_printf(0, "Counter: %d\r\n", i);
        //cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
        gpio_put(PWM_CHANNEL_1_PIN, 1 );
        sleep_ms(25);
        //cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
        gpio_put(PWM_CHANNEL_1_PIN, 0 );
        sleep_ms(25);
    }
}
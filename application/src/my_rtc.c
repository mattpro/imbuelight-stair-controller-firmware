#include <stdio.h>
#include "config.h"
#include "hardware/rtc.h"
#include "pico/stdlib.h"
#include "pico/util/datetime.h"



void RTC_init(void)
{
    datetime_t t = {
            .year  = 2024,
            .month = 05,
            .day   = 24,
            .dotw  = 5, // 0 is Sunday, so 5 is Friday
            .hour  = 11,
            .min   = 41,
            .sec   = 00
    };

    // Start the RTC
    rtc_init();
    rtc_set_datetime(&t);

    // clk_sys is >2000x faster than clk_rtc, so datetime is not updated immediately when rtc_get_datetime() is called.
    // tbe delay is up to 3 RTC clock cycles (which is 64us with the default clock settings)
    sleep_us(64);

}


void RTC_print_date_and_time(void)
{
    datetime_t t;
    char datetime_buf[256];
    char *datetime_str = &datetime_buf[0];
    static uint32_t counter = 0;

    counter ++;
    rtc_get_datetime(&t);
    datetime_to_str(datetime_str, sizeof(datetime_buf), &t);
    SEGGER_RTT_printf(0, "[%3d] %s\r\n", counter, datetime_str);
    //printf("\r%s      ", datetime_str);
}
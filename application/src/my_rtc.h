#ifndef _MY_RTC_H_
#define _MY_RTC_H_

#include <stdint.h>
#include "pico/stdlib.h"

void RTC_init(void);
void RTC_print_date_and_time(void);

#endif

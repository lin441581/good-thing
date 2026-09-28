#ifndef WATCH_LED_H
#define WATCH_LED_H

#include <stdint.h>

void WatchLed_Set(uint8_t enabled);
uint8_t WatchLed_IsOn(void);

#endif

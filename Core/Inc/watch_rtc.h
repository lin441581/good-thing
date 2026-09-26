#ifndef WATCH_RTC_H
#define WATCH_RTC_H

#include "rtc.h"

typedef struct
{
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
} WatchDateTime;

void WatchRtc_Init(void);
HAL_StatusTypeDef WatchRtc_Read(WatchDateTime *date_time);
HAL_StatusTypeDef WatchRtc_Write(const WatchDateTime *date_time);
uint8_t WatchRtc_Weekday(const WatchDateTime *date_time);
uint8_t WatchRtc_DaysInMonth(uint16_t year, uint8_t month);
uint8_t WatchRtc_IsValid(const WatchDateTime *date_time);

#endif

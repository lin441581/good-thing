#include "watch_rtc.h"
#include "stm32f1xx_hal_rtc_ex.h"

#define WATCH_RTC_MAGIC 0x5A5AU

static uint8_t is_leap(uint16_t year)
{
    return (uint8_t)(((year % 4U) == 0U && (year % 100U) != 0U) ||
                     (year % 400U) == 0U);
}

uint8_t WatchRtc_DaysInMonth(uint16_t year, uint8_t month)
{
    static const uint8_t days[] = { 31U, 28U, 31U, 30U, 31U, 30U,
                                    31U, 31U, 30U, 31U, 30U, 31U };

    if (month < 1U || month > 12U)
        return 0U;
    if (month == 2U && is_leap(year))
        return 29U;
    return days[month - 1U];
}

uint8_t WatchRtc_IsValid(const WatchDateTime *date_time)
{
    if (date_time == NULL || date_time->year < 2000U || date_time->year > 2099U)
        return 0U;
    if (date_time->month < 1U || date_time->month > 12U)
        return 0U;
    if (date_time->day < 1U || date_time->day > WatchRtc_DaysInMonth(date_time->year, date_time->month))
        return 0U;
    return date_time->hour < 24U && date_time->minute < 60U && date_time->second < 60U;
}

uint8_t WatchRtc_Weekday(const WatchDateTime *date_time)
{
    int year = date_time->year;
    int month = date_time->month;
    int day = date_time->day;
    int weekday;

    if (month < 3)
    {
        month += 12;
        year--;
    }
    weekday = (day + 2 * month + 3 * (month + 1) / 5 + year +
               year / 4 - year / 100 + year / 400 + 1) % 7;
    return (uint8_t)weekday; /* 0 Sunday, 1 Monday ... 6 Saturday */
}

HAL_StatusTypeDef WatchRtc_Write(const WatchDateTime *date_time)
{
    RTC_TimeTypeDef time = {0};
    RTC_DateTypeDef date = {0};

    if (!WatchRtc_IsValid(date_time))
        return HAL_ERROR;

    time.Hours = date_time->hour;
    time.Minutes = date_time->minute;
    time.Seconds = date_time->second;
    date.Year = (uint8_t)(date_time->year - 2000U);
    date.Month = date_time->month;
    date.Date = date_time->day;
    date.WeekDay = (uint8_t)(WatchRtc_Weekday(date_time) + 1U);

    if (HAL_RTC_SetTime(&hrtc, &time, RTC_FORMAT_BIN) != HAL_OK)
        return HAL_ERROR;
    if (HAL_RTC_SetDate(&hrtc, &date, RTC_FORMAT_BIN) != HAL_OK)
        return HAL_ERROR;
    HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, WATCH_RTC_MAGIC);
    return HAL_OK;
}

HAL_StatusTypeDef WatchRtc_Read(WatchDateTime *date_time)
{
    RTC_TimeTypeDef time = {0};
    RTC_DateTypeDef date = {0};

    if (date_time == NULL)
        return HAL_ERROR;
    if (HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN) != HAL_OK)
        return HAL_ERROR;
    if (HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN) != HAL_OK)
        return HAL_ERROR;

    date_time->year = (uint16_t)(2000U + date.Year);
    date_time->month = date.Month;
    date_time->day = date.Date;
    date_time->hour = time.Hours;
    date_time->minute = time.Minutes;
    date_time->second = time.Seconds;
    return WatchRtc_IsValid(date_time) ? HAL_OK : HAL_ERROR;
}

void WatchRtc_Init(void)
{
    WatchDateTime default_time = { 2026U, 1U, 1U, 0U, 0U, 0U };
    WatchDateTime current;

    if (HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR1) != WATCH_RTC_MAGIC ||
        WatchRtc_Read(&current) != HAL_OK)
    {
        (void)WatchRtc_Write(&default_time);
    }
}

#include "settings_page.h"
#include "watch_rtc.h"
#include "key.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include <stdio.h>

static const char *const field_names[] = {
    "YEAR", "MONTH", "DAY", "WEEKDAY", "HOUR", "MIN", "SEC"
};
static const char *const weekday_names[] = {
    "SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"
};

static uint16_t SettingsPage_FieldValue(const WatchDateTime *value, uint8_t field)
{
    switch (field)
    {
        case 0U: return value->year;
        case 1U: return value->month;
        case 2U: return value->day;
        case 3U: return WatchRtc_Weekday(value);
        case 4U: return value->hour;
        case 5U: return value->minute;
        default: return value->second;
    }
}

static void SettingsPage_Increment(WatchDateTime *value, uint8_t field)
{
    if (field == 0U)
    {
        value->year = value->year >= 2099U ? 2000U : (uint16_t)(value->year + 1U);
        if (value->day > WatchRtc_DaysInMonth(value->year, value->month))
            value->day = WatchRtc_DaysInMonth(value->year, value->month);
    }
    else if (field == 1U)
    {
        value->month = value->month >= 12U ? 1U : (uint8_t)(value->month + 1U);
        if (value->day > WatchRtc_DaysInMonth(value->year, value->month))
            value->day = WatchRtc_DaysInMonth(value->year, value->month);
    }
    else if (field == 2U || field == 3U)
    {
        uint8_t max_day = WatchRtc_DaysInMonth(value->year, value->month);
        value->day = value->day >= max_day ? 1U : (uint8_t)(value->day + 1U);
    }
    else if (field == 4U)
        value->hour = value->hour >= 23U ? 0U : (uint8_t)(value->hour + 1U);
    else if (field == 5U)
        value->minute = value->minute >= 59U ? 0U : (uint8_t)(value->minute + 1U);
    else
        value->second = value->second >= 59U ? 0U : (uint8_t)(value->second + 1U);
}

static void SettingsPage_Draw(const WatchDateTime *value, uint8_t field)
{
    char text[20];
    uint8_t weekday = WatchRtc_Weekday(value);
    uint16_t selected_value = SettingsPage_FieldValue(value, field);

    ssd1306_Fill(Black);
    ssd1306_SetCursor(0, 0);
    snprintf(text, sizeof(text), "%04u-%02u-%02u", value->year, value->month, value->day);
    ssd1306_WriteString(text, Font_7x10, White);
    ssd1306_SetCursor(88, 0);
    ssd1306_WriteString((char *)weekday_names[weekday], Font_7x10, White);
    ssd1306_SetCursor(15, 15);
    snprintf(text, sizeof(text), "%02u:%02u:%02u", value->hour, value->minute, value->second);
    ssd1306_WriteString(text, Font_11x18, White);
    ssd1306_SetCursor(0, 42);
    snprintf(text, sizeof(text), "%s", field_names[field]);
    ssd1306_WriteString(text, Font_7x10, White);
    ssd1306_SetCursor(0, 53);
    if (field == 3U)
        ssd1306_WriteString((char *)weekday_names[selected_value], Font_7x10, White);
    else
    {
        snprintf(text, sizeof(text), "%u", selected_value);
        ssd1306_WriteString(text, Font_7x10, White);
    }
    ssd1306_UpdateScreen();
}

void SettingsPage_Run(void)
{
    WatchDateTime value;
    uint8_t field = 0U;

    if (WatchRtc_Read(&value) != HAL_OK)
        return;
    SettingsPage_Draw(&value, field);
    while (1)
    {
        KeyEvent_t event = Key_GetEvent();
        if (event == KEY_EVENT_OK_LONG)
        {
            ssd1306_Fill(Black);
            ssd1306_UpdateScreen();
            return;
        }
        if (event == KEY_EVENT_NEXT || event == KEY_EVENT_NEXT_LONG)
        {
            SettingsPage_Increment(&value, field);
            SettingsPage_Draw(&value, field);
        }
        else if (event == KEY_EVENT_OK)
        {
            if (++field >= 7U)
            {
                (void)WatchRtc_Write(&value);
                ssd1306_Fill(Black);
                ssd1306_UpdateScreen();
                return;
            }
            SettingsPage_Draw(&value, field);
        }
        HAL_Delay(20);
    }
}

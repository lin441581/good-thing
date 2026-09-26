#include "home_page.h"
#include "menu.h"
#include "watch_rtc.h"
#include "key.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include <stdio.h>

static char weekdays[][4] = { "SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT" };

static void HomePage_Draw(const WatchDateTime *value)
{
    char line[24];
    uint8_t weekday = WatchRtc_Weekday(value);

    ssd1306_Fill(Black);
    snprintf(line, sizeof(line), "%02u:%02u:%02u", value->hour, value->minute, value->second);
    /* Font_16x26 clips the final digit on this driver; use compact font so
       all eight characters, including seconds, stay visible. */
    ssd1306_SetCursor(20, 8);
    ssd1306_WriteString(line, Font_11x18, White);

    snprintf(line, sizeof(line), "%04u-%02u-%02u", value->year, value->month, value->day);
    ssd1306_SetCursor(29, 34);
    ssd1306_WriteString(line, Font_7x10, White);

    ssd1306_SetCursor(53, 50);
    ssd1306_WriteString(weekdays[weekday], Font_7x10, White);
    ssd1306_UpdateScreen();
}

void HomePage_Run(void)
{
    WatchDateTime value;
    uint32_t last_draw = 0U;

    while (1)
    {
        uint32_t now = HAL_GetTick();
        KeyEvent_t event = Key_GetEvent();

        if (event == KEY_EVENT_OK || event == KEY_EVENT_OK_LONG)
        {
            ssd1306_Fill(Black);
            ssd1306_UpdateScreen();
            Menu_Run();
            last_draw = 0U;
        }

        if ((now - last_draw) >= 200U || last_draw == 0U)
        {
            last_draw = now;
            if (WatchRtc_Read(&value) == HAL_OK)
                HomePage_Draw(&value);
        }
        HAL_Delay(10);
    }
}

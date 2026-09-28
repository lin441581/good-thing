#include "stopwatch_page.h"
#include "watch_service.h"
#include "key.h"
#include "ssd1306.h"
#include "oled_chinese.h"
#include "ssd1306_fonts.h"
#include <stdio.h>

static uint32_t stopwatch_elapsed;
static uint32_t stopwatch_started_at;
static uint8_t stopwatch_running;

static uint32_t StopwatchPage_Elapsed(void)
{
    return stopwatch_running ? stopwatch_elapsed + HAL_GetTick() - stopwatch_started_at : stopwatch_elapsed;
}

static void StopwatchPage_Draw(void)
{
    char text[24];
    uint32_t elapsed = StopwatchPage_Elapsed();
    ssd1306_Fill(Black);
    OLED_Chinese_DrawText(0, 0, OLED_TEXT_STOPWATCH, White);
    ssd1306_SetCursor(0, 23);
    snprintf(text, sizeof(text), "%02lu:%02lu.%02lu",
             (unsigned long)(elapsed / 60000U),
             (unsigned long)((elapsed / 1000U) % 60U),
             (unsigned long)((elapsed / 10U) % 100U));
    ssd1306_WriteString(text, Font_11x18, White);
    ssd1306_SetCursor(0, 50);
    ssd1306_WriteString(stopwatch_running ? "RUN" : "PAUSE", Font_7x10, White);
    ssd1306_UpdateScreen();
}

void StopwatchPage_Run(void)
{
    uint32_t last_draw = HAL_GetTick();
    stopwatch_elapsed = 0U;
    stopwatch_started_at = 0U;
    stopwatch_running = 0U;
    StopwatchPage_Draw();
    while (1)
    {
        WatchService_Process();
        uint32_t now = HAL_GetTick();
        KeyEvent_t event = Key_GetEvent();
        if (event == KEY_EVENT_OK || event == KEY_EVENT_OK_LONG)
        {
            ssd1306_Fill(Black);
            ssd1306_UpdateScreen();
            return;
        }
        if (event == KEY_EVENT_NEXT)
        {
            if (!stopwatch_running)
            {
                stopwatch_started_at = now;
                stopwatch_running = 1U;
            }
            else
            {
                stopwatch_elapsed += now - stopwatch_started_at;
                stopwatch_running = 0U;
            }
            StopwatchPage_Draw();
        }
        else if (event == KEY_EVENT_NEXT_LONG)
        {
            stopwatch_elapsed = 0U;
            stopwatch_running = 0U;
            StopwatchPage_Draw();
        }
        if ((now - last_draw) >= 50U)
        {
            last_draw = now;
            StopwatchPage_Draw();
        }
        HAL_Delay(1);
    }
}

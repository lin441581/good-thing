#include "step_page.h"
#include "watch_service.h"
#include "key.h"
#include "ssd1306.h"
#include "step_counter.h"
#include "oled_chinese.h"
#include <stdio.h>

static void StepPage_Draw(void)
{
    char text[24];

    ssd1306_Fill(Black);
    OLED_Chinese_DrawText(0, 0, OLED_TEXT_STEP, White);

    ssd1306_SetCursor(0, 24);
    snprintf(text, sizeof(text), "%lu", (unsigned long)StepCounter_Get());
    ssd1306_WriteString(text, Font_16x26, White);

    ssd1306_UpdateScreen();
}

void StepPage_Run(void)
{
    uint32_t last_draw = HAL_GetTick();

    StepPage_Draw();

    while (1)
    {
        WatchService_Process();
        uint32_t now = HAL_GetTick();
        KeyEvent_t event = Key_GetEvent();

        if (event == KEY_EVENT_OK)
        {
            ssd1306_Fill(Black);
            ssd1306_UpdateScreen();
            return;
        }

        if (event == KEY_EVENT_NEXT_LONG)
        {
            StepCounter_Reset();
            StepPage_Draw();
        }

        if ((now - last_draw) >= 100U)
        {
            last_draw = now;
            StepPage_Draw();
        }

        HAL_Delay(1);
    }
}

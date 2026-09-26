#include "step_page.h"
#include "key.h"
#include "mpu6050.h"
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
    uint32_t last_sample = HAL_GetTick();
    uint32_t last_draw = last_sample;
    int16_t ax;
    int16_t ay;
    int16_t az;

    StepPage_Draw();

    while (1)
    {
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

        if ((now - last_sample) >= 20U)
        {
            last_sample = now;
            if (MPU6050_ReadAccel(&ax, &ay, &az) == HAL_OK)
            {
                StepCounter_Process(ax, ay, az);
            }
        }

        if ((now - last_draw) >= 100U)
        {
            last_draw = now;
            StepPage_Draw();
        }

        HAL_Delay(1);
    }
}

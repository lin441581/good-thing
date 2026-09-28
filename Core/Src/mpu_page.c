#include "mpu_page.h"
#include "watch_service.h"
#include "key.h"
#include "mpu6050.h"
#include "ssd1306.h"
#include "oled_chinese.h"
#include <stdio.h>

static int16_t accel_x;
static int16_t accel_y;
static int16_t accel_z;
static uint8_t accel_valid;

static void MpuPage_Draw(void)
{
    char text[24];

    ssd1306_Fill(Black);

    ssd1306_SetCursor(0, 0);
    ssd1306_WriteString("MPU6050", Font_7x10, White);

    ssd1306_SetCursor(0, 18);
    ssd1306_WriteString("X", Font_7x10, White);
    ssd1306_SetCursor(16, 18);
    snprintf(text, sizeof(text), "%6d", accel_x);
    ssd1306_WriteString(text, Font_7x10, White);

    ssd1306_SetCursor(0, 34);
    ssd1306_WriteString("Y", Font_7x10, White);
    ssd1306_SetCursor(16, 34);
    snprintf(text, sizeof(text), "%6d", accel_y);
    ssd1306_WriteString(text, Font_7x10, White);

    ssd1306_SetCursor(0, 50);
    ssd1306_WriteString("Z", Font_7x10, White);
    ssd1306_SetCursor(16, 50);
    snprintf(text, sizeof(text), "%6d", accel_z);
    ssd1306_WriteString(text, Font_7x10, White);

    ssd1306_UpdateScreen();
}

void MpuPage_Run(void)
{
    uint32_t last_sample = HAL_GetTick();
    uint32_t last_draw = last_sample;

    accel_x = 0;
    accel_y = 0;
    accel_z = 0;
    accel_valid = 0;
    MpuPage_Draw();

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

        if ((now - last_sample) >= 20U)
        {
            last_sample = now;
            accel_valid = (MPU6050_ReadAccel(&accel_x, &accel_y, &accel_z) == HAL_OK);
        }

        if ((now - last_draw) >= 100U)
        {
            last_draw = now;
            MpuPage_Draw();
        }

        HAL_Delay(1);
    }
}

#include "level_page.h"
#include "watch_service.h"
#include "key.h"
#include "mpu6050.h"
#include "ssd1306.h"
#include "oled_chinese.h"
#include "ssd1306_fonts.h"
#include <math.h>

#define RAD_TO_DEG 57.2957795f

static void LevelPage_Draw(float roll, float pitch, uint8_t valid)
{
    int16_t dot_x = 64;
    int16_t dot_y = 42;
    const char *status = valid ? "FLAT" : "ERR";

    if (valid)
    {
        dot_x = (int16_t)(64.0f - roll * 0.45f);
        dot_y = (int16_t)(42.0f + pitch * 0.45f);
        if (dot_x < 47) dot_x = 47;
        if (dot_x > 81) dot_x = 81;
        if (dot_y < 25) dot_y = 25;
        if (dot_y > 59) dot_y = 59;
        if (fabsf(roll) > 5.0f || fabsf(pitch) > 5.0f)
            status = "TILT";
    }

    ssd1306_Fill(Black);
    OLED_Chinese_DrawText(0, 0, OLED_TEXT_LEVEL, White);
    ssd1306_DrawCircle(64, 42, 21, White);
    ssd1306_DrawLine(43, 42, 85, 42, White);
    ssd1306_DrawLine(64, 21, 64, 63, White);
    if (valid)
        ssd1306_FillCircle(dot_x, dot_y, 4, White);
    ssd1306_SetCursor(85, 3);
    ssd1306_WriteString((char *)status, Font_7x10, White);
    ssd1306_UpdateScreen();
}

void LevelPage_Run(void)
{
    uint32_t last_sample = HAL_GetTick();
    uint32_t last_draw = last_sample;
    int16_t ax = 0, ay = 0, az = 0;
    float roll = 0.0f, pitch = 0.0f;
    uint8_t valid = 0U;

    LevelPage_Draw(roll, pitch, valid);
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
            valid = (MPU6050_ReadAccel(&ax, &ay, &az) == HAL_OK);
            if (valid)
            {
                roll = atan2f((float)ay, (float)az) * RAD_TO_DEG;
                pitch = atan2f((float)-ax,
                               sqrtf((float)ay * ay + (float)az * az)) * RAD_TO_DEG;
            }
        }
        if ((now - last_draw) >= 100U)
        {
            last_draw = now;
            LevelPage_Draw(roll, pitch, valid);
        }
        HAL_Delay(1);
    }
}

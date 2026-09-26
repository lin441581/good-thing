#include "led_page.h"
#include "key.h"
#include "ssd1306.h"
#include "oled_chinese.h"
#include "ssd1306_fonts.h"

static uint8_t led_on;

static void LedPage_Draw(void)
{
    ssd1306_Fill(Black);
    OLED_Chinese_DrawText(0, 0, OLED_TEXT_FLASHLIGHT, White);
    ssd1306_SetCursor(48, 30);
    ssd1306_WriteString(led_on ? "ON" : "OFF", Font_16x26, White);
    ssd1306_UpdateScreen();
}

void LedPage_Run(void)
{
    led_on = 0U;
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_RESET);
    LedPage_Draw();
    while (1)
    {
        KeyEvent_t event = Key_GetEvent();
        if (event == KEY_EVENT_OK || event == KEY_EVENT_OK_LONG)
        {
            ssd1306_Fill(Black);
            ssd1306_UpdateScreen();
            return;
        }
        if (event == KEY_EVENT_NEXT || event == KEY_EVENT_NEXT_LONG)
        {
            led_on = (uint8_t)!led_on;
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15,
                              led_on ? GPIO_PIN_SET : GPIO_PIN_RESET);
            LedPage_Draw();
        }
        HAL_Delay(10);
    }
}

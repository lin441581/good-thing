#include "led_page.h"
#include "key.h"
#include "ssd1306.h"
#include "oled_chinese.h"
#include "ssd1306_fonts.h"

static uint8_t led_on;
static uint8_t led_cursor;

static void LedPage_Apply(void)
{
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15,
                      led_on ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

static void LedPage_Draw(void)
{
    ssd1306_Fill(Black);
    OLED_Chinese_DrawText(0, 0, OLED_TEXT_FLASHLIGHT, White);
    ssd1306_SetCursor(25, 30);
    ssd1306_WriteString("OFF", Font_11x18, White);
    ssd1306_SetCursor(79, 30);
    ssd1306_WriteString("ON", Font_11x18, White);
    if (led_cursor == 0U)
        ssd1306_DrawRect(20, 26, 43, 24, White);
    else
        ssd1306_DrawRect(74, 26, 33, 24, White);
    ssd1306_UpdateScreen();
}

void LedPage_Run(void)
{
    led_on = 0U;
    led_cursor = 0U;
    LedPage_Apply();
    LedPage_Draw();
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
            led_cursor = (uint8_t)!led_cursor;
            LedPage_Draw();
        }
        else if (event == KEY_EVENT_OK)
        {
            led_on = led_cursor;
            LedPage_Apply();
            LedPage_Draw();
        }
        HAL_Delay(10);
    }
}

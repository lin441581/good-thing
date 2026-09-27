#include "game_menu.h"
#include "game_page.h"
#include "game2_page.h"
#include "key.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include "oled_chinese.h"

static const uint8_t game_title[5][32] = {
 {0x0C,0x30,0x18,0x0C,0x61,0x84,0x03,0x80,0x06,0xC0,0x0C,0x70,0x18,0x1C,0x7F,0xFE,0x08,0x10,0x08,0x10,0x08,0x10,0x0F,0xF0,0x08,0x10,0x08,0x00,0,0,0,0},
 {0x7F,0xA0,0x03,0x20,0x3B,0x7E,0x2B,0x46,0x3B,0xC4,0x00,0x10,0x7F,0x90,0x03,0x10,0x3B,0x10,0x2B,0x28,0x3B,0x2C,0x2B,0x46,0x66,0xC2,0x04,0,0,0,0,0},
 {0x01,0x80,0x01,0x80,0x01,0x80,0x09,0x90,0x19,0x98,0x11,0x88,0x11,0x8C,0x31,0x84,0x21,0x84,0x61,0x84,0x01,0x80,0x01,0x80,0x03,0x80,0,0,0,0,0,0},
 {0x18,0x98,0x18,0xD8,0x18,0xF8,0x1B,0x38,0x7D,0x1A,0x43,0x0E,0x02,0x00,0x14,0x88,0x34,0xCC,0x24,0x06,0x64,0x10,0x07,0xF0,0,0,0,0,0,0,0,0},
 {0x03,0x18,0x03,0x00,0x7F,0xFE,0x02,0xC0,0x02,0xC0,0x02,0xC8,0x06,0xD8,0x04,0xF0,0x0C,0xE0,0x18,0xC0,0x31,0xC2,0x66,0xFE,0,0,0,0,0,0,0,0}
};

static void GameMenu_DrawTitle(void)
{
    for (uint8_t c = 0; c < 5U; c++)
        for (uint8_t row = 0; row < 16U; row++)
            for (uint8_t col = 0; col < 16U; col++)
                if (game_title[c][row * 2U + col / 8U] & (uint8_t)(0x80U >> (col % 8U)))
                    ssd1306_DrawPixel((uint8_t)(24U + c * 16U + col), (uint8_t)(4U + row), White);
}

static void GameMenu_DrawSelectionBox(uint8_t x, uint8_t y,
                                      uint8_t width, uint8_t height)
{
    ssd1306_DrawLine(x, y, (uint8_t)(x + width), y, White);
    ssd1306_DrawLine(x, (uint8_t)(y + height),
                     (uint8_t)(x + width), (uint8_t)(y + height), White);
    ssd1306_DrawLine(x, y, x, (uint8_t)(y + height), White);
    ssd1306_DrawLine((uint8_t)(x + width), y,
                     (uint8_t)(x + width), (uint8_t)(y + height), White);
}

static void GameMenu_DrawGame2(uint8_t selected)
{
    OLED_Chinese_DrawText(24, 40, OLED_TEXT_COIN, White);
    if (selected)
        GameMenu_DrawSelectionBox(20, 37, 88, 23);
}

void GameMenu_Run(void)
{
    uint8_t selected = 0U;
    while (1)
    {
        ssd1306_Fill(Black);
        GameMenu_DrawTitle();
        if (!selected)
            GameMenu_DrawSelectionBox(20, 1, 88, 22);
        GameMenu_DrawGame2(selected);
        ssd1306_UpdateScreen();

        KeyEvent_t event = Key_GetEvent();
        if (event == KEY_EVENT_OK || event == KEY_EVENT_OK_LONG)
        {
            if (event == KEY_EVENT_OK && selected == 0U)
                GamePage_Run();
            else if (event == KEY_EVENT_OK && selected == 1U)
                Game2Page_Run();
            else if (event == KEY_EVENT_OK_LONG)
            {
                ssd1306_Fill(Black);
                ssd1306_UpdateScreen();
                return;
            }
        }
        else if (event == KEY_EVENT_NEXT)
            selected = (uint8_t)!selected;
        HAL_Delay(10);
    }
}

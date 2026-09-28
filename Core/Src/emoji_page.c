#include "emoji_page.h"
#include "watch_service.h"
#include "key.h"
#include "ssd1306.h"
#include "oled_assets.h"

static const uint8_t *const emoji_frames[] = { emoji_frame_3, emoji_frame_4 };

static void EmojiPage_DrawScaled(const uint8_t *bitmap)
{
    /* Scale complete 32x32 source to the largest square fitting 128x64. */
    for (uint16_t y = 0; y < 64U; y++)
    {
        uint16_t sy = (uint16_t)(y * ICON_HEIGHT / 64U);
        for (uint16_t x = 0; x < 64U; x++)
        {
            uint16_t sx = (uint16_t)(x * ICON_WIDTH / 64U);
            uint16_t byte_index = (uint16_t)(sy * (ICON_WIDTH / 8U) + sx / 8U);
            if ((bitmap[byte_index] & (uint8_t)(0x80U >> (sx % 8U))) != 0U)
                ssd1306_DrawPixel((uint8_t)(32U + x), (uint8_t)y, White);
        }
    }
}

static void EmojiPage_Draw(uint8_t selected)
{
    ssd1306_Fill(Black);
    EmojiPage_DrawScaled(emoji_frames[selected]);
    ssd1306_UpdateScreen();
}

void EmojiPage_Run(void)
{
    uint8_t selected = 0U;
    EmojiPage_Draw(selected);
    while (1)
    {
        WatchService_Process();
        KeyEvent_t event = Key_GetEvent();
        if (event == KEY_EVENT_OK || event == KEY_EVENT_OK_LONG)
        {
            ssd1306_Fill(Black);
            ssd1306_UpdateScreen();
            return;
        }
        if (event == KEY_EVENT_NEXT || event == KEY_EVENT_NEXT_LONG)
        {
            selected = (uint8_t)((selected + 1U) % 2U);
            EmojiPage_Draw(selected);
        }
        HAL_Delay(10);
    }
}

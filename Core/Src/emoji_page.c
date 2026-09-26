#include "emoji_page.h"
#include "key.h"
#include "ssd1306.h"

static void EmojiPage_Draw(void)
{
    ssd1306_Fill(Black);
    /* Simple face placeholder; replace with user animation frames later. */
    ssd1306_DrawCircle(64, 32, 20, White);
    ssd1306_DrawCircle(57, 27, 2, White);
    ssd1306_DrawCircle(71, 27, 2, White);
    ssd1306_DrawLine(56, 40, 72, 40, White);
    ssd1306_UpdateScreen();
}

void EmojiPage_Run(void)
{
    EmojiPage_Draw();
    while (1)
    {
        KeyEvent_t event = Key_GetEvent();
        if (event == KEY_EVENT_OK || event == KEY_EVENT_OK_LONG)
        {
            ssd1306_Fill(Black);
            ssd1306_UpdateScreen();
            return;
        }
        HAL_Delay(10);
    }
}

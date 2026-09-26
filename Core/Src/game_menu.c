#include "game_menu.h"
#include "game_page.h"
#include "key.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"

void GameMenu_Run(void)
{
    uint8_t selected = 0U;
    while (1)
    {
        ssd1306_Fill(Black);
        ssd1306_SetCursor(8, 8);
        ssd1306_WriteString("GOOGLE DINO", Font_11x18, White);
        ssd1306_DrawRect(4, 5, 120, 28, White);
        ssd1306_SetCursor(36, 45);
        ssd1306_WriteString(selected ? "ON" : "PLAY", Font_7x10, White);
        ssd1306_UpdateScreen();

        KeyEvent_t event = Key_GetEvent();
        if (event == KEY_EVENT_OK || event == KEY_EVENT_OK_LONG)
        {
            if (event == KEY_EVENT_OK)
                GamePage_Run();
            else
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

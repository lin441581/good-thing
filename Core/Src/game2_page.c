#include "game2_page.h"
#include "watch_service.h"
#include "key.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include <stdio.h>

#define GAME2_TICK_MS       30U
#define GAME2_PLAYER_Y      55
#define GAME2_PLAYER_W      22
#define GAME2_PLAYER_H      4
#define GAME2_COIN_R        4
#define GAME2_COIN_START_Y  10
#define GAME2_COIN_SPEED     1

static uint32_t game2_seed = 0x13579BDFU;

static uint32_t Game2_NextRandom(void)
{
    game2_seed = game2_seed * 1664525U + 1013904223U;
    return game2_seed;
}

static void Game2_Reset(int16_t *player_x, int8_t *direction,
                        int16_t *coin_x, int16_t *coin_y,
                        uint16_t *score)
{
    *player_x = (SSD1306_WIDTH - GAME2_PLAYER_W) / 2;
    *direction = 1;
    *coin_x = (int16_t)(8U + (Game2_NextRandom() % (SSD1306_WIDTH - 16U)));
    *coin_y = GAME2_COIN_START_Y;
    *score = 0;
}

static void Game2_NewCoin(int16_t *coin_x, int16_t *coin_y)
{
    *coin_x = (int16_t)(8U + (Game2_NextRandom() % (SSD1306_WIDTH - 16U)));
    *coin_y = GAME2_COIN_START_Y;
}

static void Game2_Draw(int16_t player_x, int16_t coin_x, int16_t coin_y,
                       uint16_t score)
{
    char score_text[8];

    ssd1306_Fill(Black);
    ssd1306_DrawLine(0, 0, 127, 0, White);
    ssd1306_FillRect((uint16_t)player_x, GAME2_PLAYER_Y,
                     GAME2_PLAYER_W, GAME2_PLAYER_H, White);
    ssd1306_DrawCircle(coin_x, coin_y, GAME2_COIN_R, White);
    ssd1306_DrawPixel((uint8_t)coin_x, (uint8_t)coin_y, White);

    snprintf(score_text, sizeof(score_text), "%u", score);
    ssd1306_SetCursor(108, 3);
    ssd1306_WriteString(score_text, Font_7x10, White);
    ssd1306_UpdateScreen();
}

void Game2Page_Run(void)
{
    int16_t player_x;
    int8_t direction;
    int16_t coin_x;
    int16_t coin_y;
    uint16_t score;
    uint32_t last_tick = HAL_GetTick();
    uint32_t last_draw = last_tick;

    Game2_Reset(&player_x, &direction, &coin_x, &coin_y, &score);
    Game2_Draw(player_x, coin_x, coin_y, score);

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
        if (event == KEY_EVENT_NEXT)
            direction = (int8_t)-direction;
        else if (event == KEY_EVENT_NEXT_LONG)
        {
            Game2_Reset(&player_x, &direction, &coin_x, &coin_y, &score);
            last_tick = now;
            last_draw = now;
        }

        if ((now - last_tick) >= GAME2_TICK_MS)
        {
            last_tick = now;
            player_x += direction;
            if (player_x <= 0 || player_x >= (SSD1306_WIDTH - GAME2_PLAYER_W))
            {
                player_x = player_x <= 0 ? 0 : SSD1306_WIDTH - GAME2_PLAYER_W;
                direction = (int8_t)-direction;
            }

            coin_y += GAME2_COIN_SPEED;
            if (coin_y + GAME2_COIN_R >= GAME2_PLAYER_Y &&
                coin_y - GAME2_COIN_R <= GAME2_PLAYER_Y + GAME2_PLAYER_H &&
                coin_x + GAME2_COIN_R >= player_x &&
                coin_x - GAME2_COIN_R <= player_x + GAME2_PLAYER_W)
            {
                score++;
                Game2_NewCoin(&coin_x, &coin_y);
            }
            else if (coin_y - GAME2_COIN_R > SSD1306_HEIGHT)
            {
                Game2_NewCoin(&coin_x, &coin_y);
            }
        }

        if ((now - last_draw) >= 60U)
        {
            last_draw = now;
            Game2_Draw(player_x, coin_x, coin_y, score);
        }
        HAL_Delay(1);
    }
}

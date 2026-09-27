#include "game_page.h"
#include "key.h"
#include "ssd1306.h"
#include "oled_assets.h"
#include "oled_chinese.h"
#include <stdio.h>
#include <math.h>
#include <string.h>

#define GROUND_Y         62
#define DINO_X           0
#define DINO_W           16
#define DINO_H           18
#define OBSTACLE_W       16
#define OBSTACLE_H       18
#define DINO_HIT_X       2
#define DINO_HIT_W       12
#define DINO_HIT_H       14
#define OBSTACLE_HIT_X   3
#define OBSTACLE_HIT_W   10
#define OBSTACLE_HIT_H   14
#define GAME_TICK_MS     20U
#define OBSTACLE_STEP_MS 20U
#define SPEED_INTERVAL_MS 10000U
#define SPEED_STEP_PERCENT 10U
#define SPEED_MAX_PERCENT 200U
#define JUMP_DURATION_MS 1000U
#define JUMP_HEIGHT      28

static int16_t obstacle_x;
static uint16_t score;
static uint16_t jump_elapsed_ms;
static uint32_t obstacle_move_units;
static uint32_t game_elapsed_ms;
static uint8_t game_over;

static void GamePage_FillRectClipped(int16_t x, int16_t y,
                                     uint16_t width, uint16_t height)
{
    int16_t left = x < 0 ? 0 : x;
    int16_t top = y < 0 ? 0 : y;
    int16_t right = x + (int16_t)width;
    int16_t bottom = y + (int16_t)height;

    if (right > SSD1306_WIDTH)
        right = SSD1306_WIDTH;
    if (bottom > SSD1306_HEIGHT)
        bottom = SSD1306_HEIGHT;

    if (left < right && top < bottom)
    {
        ssd1306_FillRect((uint16_t)left, (uint16_t)top,
                         (uint16_t)(right - left),
                         (uint16_t)(bottom - top), White);
    }
}

static void GamePage_DrawCactus(int16_t x)
{
    GamePage_FillRectClipped(x + 6, GROUND_Y - OBSTACLE_H, 4, OBSTACLE_H);
    GamePage_FillRectClipped(x + 2, GROUND_Y - 12, 4, 4);
    GamePage_FillRectClipped(x + 2, GROUND_Y - 16, 3, 5);
    GamePage_FillRectClipped(x + 10, GROUND_Y - 9, 4, 4);
    GamePage_FillRectClipped(x + 11, GROUND_Y - 13, 3, 5);
}

static void GamePage_Reset(void)
{
    obstacle_x = 120;
    score = 0;
    jump_elapsed_ms = 0;
    obstacle_move_units = 0;
    game_elapsed_ms = 0;
    game_over = 0;
}

static int16_t GamePage_DinoY(void)
{
    if (jump_elapsed_ms == 0)
        return GROUND_Y - DINO_H;

    return (int16_t)(GROUND_Y - DINO_H -
        (int16_t)(JUMP_HEIGHT * sinf(3.1415926f * jump_elapsed_ms /
                                    (float)JUMP_DURATION_MS)));
}

static uint8_t GamePage_Collision(void)
{
    int16_t dino_y = GamePage_DinoY();
    int16_t dino_left = DINO_X + DINO_HIT_X;
    int16_t dino_right = dino_left + DINO_HIT_W;
    int16_t dino_bottom = dino_y + DINO_H - (DINO_H - DINO_HIT_H);
    int16_t obstacle_left = obstacle_x + OBSTACLE_HIT_X;
    int16_t obstacle_right = obstacle_left + OBSTACLE_HIT_W;
    int16_t obstacle_top = GROUND_Y - OBSTACLE_HIT_H;

    if (obstacle_x < 0 || obstacle_x >= 128)
        return 0;

    return dino_right > obstacle_left &&
           dino_left < obstacle_right &&
           dino_bottom > obstacle_top;
}

static void GamePage_Draw(void)
{
    char text[20];
    char score_text[8];
    int16_t dino_y = GamePage_DinoY();

    ssd1306_Fill(Black);
    ssd1306_DrawLine(0, GROUND_Y, 127, GROUND_Y, White);

    ssd1306_DrawBitmap(DINO_X, (uint16_t)dino_y, dino_frame, DINO_WIDTH, DINO_HEIGHT, White);
    if (obstacle_x > -OBSTACLE_W && obstacle_x < SSD1306_WIDTH)
    {
        GamePage_DrawCactus(obstacle_x);
    }

    snprintf(score_text, sizeof(score_text), "%u", score);
    OLED_Chinese_DrawText(0, 0, OLED_TEXT_SCORE, White);
    ssd1306_SetCursor(32, 3);
    ssd1306_WriteString(score_text, Font_7x10, White);

    snprintf(text, sizeof(text), "%lu.%lu",
             (unsigned long)(game_elapsed_ms / 1000U),
             (unsigned long)((game_elapsed_ms / 100U) % 10U));
    ssd1306_SetCursor(78, 3);
    ssd1306_WriteString(text, Font_7x10, White);
    ssd1306_WriteString("s", Font_7x10, White);

    if (game_over)
    {
        OLED_Chinese_DrawText(48, 24, OLED_TEXT_GAME_OVER, White);
    }

    ssd1306_UpdateScreen();
}

void GamePage_Run(void)
{
    uint32_t last_tick = HAL_GetTick();
    uint32_t last_draw = last_tick;

    GamePage_Reset();
    GamePage_Draw();

    while (1)
    {
        uint32_t now = HAL_GetTick();
        KeyEvent_t event = Key_GetEvent();

        if (event == KEY_EVENT_OK || event == KEY_EVENT_OK_LONG)
        {
            ssd1306_Fill(Black);
            ssd1306_UpdateScreen();
            return;
        }

        if (game_over)
        {
            if (event == KEY_EVENT_NEXT_LONG)
            {
                GamePage_Reset();
                last_tick = now;
                last_draw = now;
                GamePage_Draw();
            }
        }
        else
        {
            if (event == KEY_EVENT_NEXT && jump_elapsed_ms == 0)
                jump_elapsed_ms = 1;

            if (event == KEY_EVENT_NEXT_LONG)
            {
                GamePage_Reset();
                last_tick = now;
                last_draw = now;
                GamePage_Draw();
            }

            if ((now - last_tick) >= GAME_TICK_MS)
            {
                uint32_t elapsed = now - last_tick;
                last_tick = now;
                uint32_t speed_percent;

                game_elapsed_ms += elapsed;
                speed_percent = SPEED_STEP_PERCENT *
                    (game_elapsed_ms / SPEED_INTERVAL_MS) + 100U;
                if (speed_percent > SPEED_MAX_PERCENT)
                    speed_percent = SPEED_MAX_PERCENT;

                if (jump_elapsed_ms > 0)
                {
                    if (jump_elapsed_ms + elapsed >= JUMP_DURATION_MS)
                        jump_elapsed_ms = 0;
                    else
                        jump_elapsed_ms += (uint16_t)elapsed;
                }

                /* Fixed-point movement: 100% = one pixel per base interval. */
                obstacle_move_units += elapsed * speed_percent;
                while (obstacle_move_units >= OBSTACLE_STEP_MS * 100U)
                {
                    obstacle_move_units -= OBSTACLE_STEP_MS * 100U;
                    obstacle_x--;

                    if (obstacle_x < -OBSTACLE_W)
                    {
                        obstacle_x = 120;
                        score++;
                    }
                }

                if (GamePage_Collision())
                    game_over = 1;
            }
        }

        if ((now - last_draw) >= 80U)
        {
            last_draw = now;
            GamePage_Draw();
        }

        HAL_Delay(1);
    }
}

#include "menu.h"
#include "key.h"
#include "ssd1306.h"
#include "step_page.h"
#include "mpu_page.h"
#include "level_page.h"
#include "led_page.h"
#include "stopwatch_page.h"
#include "game_page.h"
#include "battery_page.h"
#include "settings_page.h"
#include "emoji_page.h"
#include "game_menu.h"
#include "oled_assets.h"
#include "oled_chinese.h"
#include <stdio.h>

#define MENU_COUNT 9
#define MENU_VISIBLE_ITEMS 3

typedef enum { MENU_ITEM_STOPWATCH, MENU_ITEM_FLASHLIGHT,
               MENU_ITEM_MPU6050, MENU_ITEM_GAME,
               MENU_ITEM_LEVEL, MENU_ITEM_STEP, MENU_ITEM_BATTERY,
               MENU_ITEM_SETTINGS, MENU_ITEM_EMOJI } MenuItem;

static uint8_t menu_index;
static const uint8_t menu_items[MENU_COUNT] = {
    MENU_ITEM_STOPWATCH, MENU_ITEM_FLASHLIGHT, MENU_ITEM_MPU6050,
    MENU_ITEM_GAME, MENU_ITEM_LEVEL, MENU_ITEM_EMOJI,
    MENU_ITEM_STEP, MENU_ITEM_BATTERY, MENU_ITEM_SETTINGS
};
static const OledChineseTextId menu_name[MENU_COUNT] = {
    OLED_TEXT_STOPWATCH, OLED_TEXT_FLASHLIGHT, OLED_TEXT_ACCELERATION,
    OLED_TEXT_GAME, OLED_TEXT_LEVEL, OLED_TEXT_GAME,
    OLED_TEXT_STEP, OLED_TEXT_BATTERY, OLED_TEXT_BATTERY
};
static const uint8_t *const menu_icons[MENU_COUNT] = {
    icon_stopwatch, icon_led, icon_mpu6050, icon_game,
    icon_level, icon_emoji_smile, icon_step, icon_battery, icon_settings
};

static void Menu_Draw(void)
{
    ssd1306_Fill(Black);
    for (int8_t offset = -1; offset <= 1; offset++)
    {
        int16_t index = (int16_t)menu_index + offset;
        int16_t x = (offset + 1) * 42 + 5;
        if (index < 0) index += MENU_COUNT;
        if (index >= MENU_COUNT) index -= MENU_COUNT;
        ssd1306_DrawBitmap((uint16_t)x, 16, menu_icons[index], ICON_WIDTH, ICON_HEIGHT, White);
        if (offset == 0)
            ssd1306_DrawRect((uint16_t)(x - 2), 13, 36, 38, White);
        if (menu_items[index] == MENU_ITEM_EMOJI)
        {
            ssd1306_SetCursor((uint8_t)(x + 1), 51);
            ssd1306_WriteString("EMOJI", Font_7x10, White);
        }
        else if (menu_items[index] == MENU_ITEM_SETTINGS)
        {
            ssd1306_SetCursor((uint8_t)(x + 4), 52);
            ssd1306_WriteString("SET", Font_7x10, White);
        }
        else
            OLED_Chinese_DrawTextSmall((uint8_t)(x + (32U - OLED_Chinese_TextWidthSmall(menu_name[index])) / 2U),
                                       52, menu_name[index], White);
    }
    ssd1306_UpdateScreen();
}

static void Menu_Enter(uint8_t index)
{
    switch (menu_items[index])
    {
        case MENU_ITEM_STOPWATCH: StopwatchPage_Run(); break;
        case MENU_ITEM_MPU6050: MpuPage_Run(); break;
        case MENU_ITEM_LEVEL: LevelPage_Run(); break;
        case MENU_ITEM_FLASHLIGHT: LedPage_Run(); break;
        case MENU_ITEM_GAME: GameMenu_Run(); break;
        case MENU_ITEM_STEP: StepPage_Run(); break;
        case MENU_ITEM_BATTERY: BatteryPage_Run(); break;
        case MENU_ITEM_SETTINGS: SettingsPage_Run(); break;
        case MENU_ITEM_EMOJI: EmojiPage_Run(); break;
        default: break;
    }
}

void Menu_Init(void) { menu_index = 0; }

void Menu_Run(void)
{
    Menu_Init();
    Menu_Draw();
    while (1)
    {
        KeyEvent_t event = Key_GetEvent();
        if (event == KEY_EVENT_NEXT)
        {
            menu_index = (uint8_t)((menu_index + 1U) % MENU_COUNT);
            Menu_Draw();
        }
        else if (event == KEY_EVENT_OK)
        {
            Menu_Enter(menu_index);
            Menu_Draw();
        }
        else if (event == KEY_EVENT_OK_LONG)
            return;
        HAL_Delay(10);
    }
}

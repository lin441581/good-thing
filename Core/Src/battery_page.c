#include "battery_page.h"
#include "watch_service.h"
#include "adc.h"
#include "key.h"
#include "ssd1306.h"
#include "oled_chinese.h"
#include <stdio.h>

/* Set to the resistor-divider ratio from battery pin to PA0. */
#define BATTERY_DIVIDER_RATIO 2.0f
#define ADC_REFERENCE_MV      3300U
#define ADC_FULL_SCALE        4095U

static uint16_t BatteryPage_ReadRaw(void)
{
    uint32_t sum = 0;

    for (uint8_t i = 0; i < 16; i++)
    {
        HAL_ADC_Start(&hadc1);
        if (HAL_ADC_PollForConversion(&hadc1, 5) == HAL_OK)
            sum += HAL_ADC_GetValue(&hadc1);
        HAL_ADC_Stop(&hadc1);
    }

    return (uint16_t)(sum / 16U);
}

static uint8_t BatteryPage_Percent(uint16_t raw)
{
    int32_t percent = ((int32_t)raw - 3276) * 100 / 819;

    if (percent < 0)
        percent = 0;
    if (percent > 100)
        percent = 100;

    return (uint8_t)percent;
}

static void BatteryPage_Draw(uint16_t raw)
{
    char text[24];
    uint32_t pin_mv = (uint32_t)raw * ADC_REFERENCE_MV / ADC_FULL_SCALE;
    uint32_t battery_mv = (uint32_t)(pin_mv * BATTERY_DIVIDER_RATIO);

    ssd1306_Fill(Black);
    OLED_Chinese_DrawText(0, 0, OLED_TEXT_BATTERY, White);

    ssd1306_SetCursor(0, 18);
    snprintf(text, sizeof(text), "%lu.%02luV",
             (unsigned long)(battery_mv / 1000U),
             (unsigned long)((battery_mv % 1000U) / 10U));
    ssd1306_WriteString(text, Font_16x26, White);
    OLED_Chinese_DrawText(0, 48, OLED_TEXT_BATTERY_LEVEL, White);
    ssd1306_SetCursor(40, 51);
    snprintf(text, sizeof(text), "%u%%", BatteryPage_Percent(raw));
    ssd1306_WriteString(text, Font_7x10, White);
    ssd1306_UpdateScreen();
}

void BatteryPage_Run(void)
{
    uint16_t raw;

    /* Original hardware uses active-low BAT_ADC_EN. */
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
    HAL_Delay(2);
    raw = BatteryPage_ReadRaw();
    BatteryPage_Draw(raw);

    while (1)
    {
        WatchService_Process();
        KeyEvent_t event = Key_GetEvent();

        if (event == KEY_EVENT_OK || event == KEY_EVENT_OK_LONG)
        {
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
            ssd1306_Fill(Black);
            ssd1306_UpdateScreen();
            return;
        }

        if (event == KEY_EVENT_NEXT)
        {
            raw = BatteryPage_ReadRaw();
            BatteryPage_Draw(raw);
        }

        HAL_Delay(20);
    }
}

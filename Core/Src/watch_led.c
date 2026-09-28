#include "watch_led.h"
#include "main.h"

#define WATCH_LED_GPIO_PORT GPIOB
#define WATCH_LED_GPIO_PIN  GPIO_PIN_15

void WatchLed_Set(uint8_t enabled)
{
    HAL_GPIO_WritePin(WATCH_LED_GPIO_PORT, WATCH_LED_GPIO_PIN,
                      enabled ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

uint8_t WatchLed_IsOn(void)
{
    return HAL_GPIO_ReadPin(WATCH_LED_GPIO_PORT, WATCH_LED_GPIO_PIN) == GPIO_PIN_SET;
}

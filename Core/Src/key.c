#include "key.h"

#define KEY_NEXT_MASK   0x01U
#define KEY_OK_MASK     0x02U

#define DEBOUNCE_TICKS  3U
#define LONG_PRESS_TICKS 100U

static volatile KeyEvent_t key_event = KEY_EVENT_NONE;
static uint8_t stable_state;
static uint8_t sample_state;
static uint8_t debounce_ticks;
static uint16_t hold_ticks[2];
static uint8_t long_sent[2];

static uint8_t Key_ReadRaw(void)
{
    uint8_t state = 0;

    if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_6) == GPIO_PIN_RESET)
        state |= KEY_NEXT_MASK;

    if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4) == GPIO_PIN_RESET)
        state |= KEY_OK_MASK;

    return state;
}

static void Key_PostEvent(KeyEvent_t event)
{
    if (key_event == KEY_EVENT_NONE)
        key_event = event;
}

void Key_Init(void)
{
    stable_state = 0;
    sample_state = 0;
    debounce_ticks = 0;

    for (uint8_t i = 0; i < 2; i++)
    {
        hold_ticks[i] = 0;
        long_sent[i] = 0;
    }
}

void Key_Tick10ms(void)
{
    uint8_t raw = Key_ReadRaw();

    if (raw != sample_state)
    {
        sample_state = raw;
        debounce_ticks = 0;
        return;
    }

    if (debounce_ticks < DEBOUNCE_TICKS)
    {
        debounce_ticks++;
        return;
    }

    if (stable_state != sample_state)
    {
        uint8_t changed = stable_state ^ sample_state;

        for (uint8_t i = 0; i < 2; i++)
        {
            uint8_t mask = (uint8_t)(1U << i);

            if ((changed & mask) == 0)
                continue;

            if (sample_state & mask)
            {
                hold_ticks[i] = 0;
                long_sent[i] = 0;
            }
            else if (!long_sent[i])
            {
                Key_PostEvent((KeyEvent_t)(i + 1));
            }
        }

        stable_state = sample_state;
    }

    for (uint8_t i = 0; i < 2; i++)
    {
        uint8_t mask = (uint8_t)(1U << i);

        if (stable_state & mask)
        {
            if (hold_ticks[i] < LONG_PRESS_TICKS)
                hold_ticks[i]++;

            if (hold_ticks[i] >= LONG_PRESS_TICKS && !long_sent[i])
            {
                Key_PostEvent(i == 0 ? KEY_EVENT_NEXT_LONG : KEY_EVENT_OK_LONG);
                long_sent[i] = 1;
            }
        }
    }
}

KeyEvent_t Key_GetEvent(void)
{
    KeyEvent_t event = key_event;
    key_event = KEY_EVENT_NONE;
    return event;
}

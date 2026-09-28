#include "bluetooth.h"
#include "step_counter.h"
#include "usart.h"
#include "watch_rtc.h"
#include "watch_led.h"
#include <stdio.h>
#include <string.h>

#define BLUETOOTH_COMMAND_SIZE 40U

static char command[BLUETOOTH_COMMAND_SIZE];
static uint8_t command_length;
static uint8_t discard_command;
static uint32_t last_sent_steps;

static HAL_StatusTypeDef Bluetooth_Send(const char *text)
{
    return HAL_UART_Transmit(&huart1, (uint8_t *)text,
                             (uint16_t)strlen(text), 100U);
}

static uint8_t Bluetooth_ParseDigits(const char *text, uint8_t min_count,
                                     uint8_t max_count, uint16_t *value)
{
    uint16_t parsed = 0U;
    uint8_t count = 0U;

    while (text[count] != '\0' && count < max_count)
    {
        if (text[count] < '0' || text[count] > '9')
            return 0U;
        parsed = (uint16_t)(parsed * 10U + (uint16_t)(text[count] - '0'));
        count++;
    }

    if (count < min_count || text[count] != '\0')
        return 0U;

    *value = parsed;
    return 1U;
}

static uint8_t Bluetooth_FieldMatches(const char *field, uint8_t length,
                                      const char *expected)
{
    if (strlen(expected) != length)
        return 0U;

    for (uint8_t i = 0U; i < length; i++)
    {
        char character = field[i];
        if (character >= 'A' && character <= 'Z')
            character = (char)(character - 'A' + 'a');
        if (character != expected[i])
            return 0U;
    }

    return 1U;
}

static uint8_t Bluetooth_CommandEquals(const char *expected)
{
    uint8_t i = 0U;

    while (command[i] != '\0' && expected[i] != '\0')
    {
        char character = command[i];
        if (character >= 'a' && character <= 'z')
            character = (char)(character - 'a' + 'A');
        if (character != expected[i])
            return 0U;
        i++;
    }

    return command[i] == '\0' && expected[i] == '\0';
}

static void Bluetooth_HandleCommand(void)
{
    WatchDateTime value;
    uint16_t parsed;
    char *separator;
    const char *field_name = NULL;
    uint8_t field_length;
    uint8_t min_digits;
    uint8_t max_digits;

    command[command_length] = '\0';
    separator = strchr(command, ':');
    if (Bluetooth_CommandEquals("LED ON"))
    {
        WatchLed_Set(1U);
        (void)Bluetooth_Send("on\r\n");
    }
    else if (Bluetooth_CommandEquals("LED OFF"))
    {
        WatchLed_Set(0U);
        (void)Bluetooth_Send("off\r\n");
    }
    else if (separator == NULL || separator == command)
        (void)Bluetooth_Send("ERR,CMD\r\n");
    else
    {
        field_length = (uint8_t)(separator - command);
        if (Bluetooth_FieldMatches(command, field_length, "year"))
        {
            field_name = "YEAR";
            min_digits = 4U;
            max_digits = 4U;
        }
        else if (Bluetooth_FieldMatches(command, field_length, "month"))
        {
            field_name = "MONTH";
            min_digits = 1U;
            max_digits = 2U;
        }
        else if (Bluetooth_FieldMatches(command, field_length, "day"))
        {
            field_name = "DAY";
            min_digits = 1U;
            max_digits = 2U;
        }
        else if (Bluetooth_FieldMatches(command, field_length, "hour"))
        {
            field_name = "HOUR";
            min_digits = 1U;
            max_digits = 2U;
        }
        else if (Bluetooth_FieldMatches(command, field_length, "minute"))
        {
            field_name = "MINUTE";
            min_digits = 1U;
            max_digits = 2U;
        }
        else if (Bluetooth_FieldMatches(command, field_length, "second"))
        {
            field_name = "SECOND";
            min_digits = 1U;
            max_digits = 2U;
        }

        if (field_name == NULL ||
            !Bluetooth_ParseDigits(separator + 1, min_digits, max_digits, &parsed) ||
            WatchRtc_Read(&value) != HAL_OK)
        {
            (void)Bluetooth_Send("ERR,VALUE\r\n");
            return;
        }

        if (strcmp(field_name, "YEAR") == 0) value.year = parsed;
        else if (strcmp(field_name, "MONTH") == 0) value.month = (uint8_t)parsed;
        else if (strcmp(field_name, "DAY") == 0) value.day = (uint8_t)parsed;
        else if (strcmp(field_name, "HOUR") == 0) value.hour = (uint8_t)parsed;
        else if (strcmp(field_name, "MINUTE") == 0) value.minute = (uint8_t)parsed;
        else value.second = (uint8_t)parsed;

        if (!WatchRtc_IsValid(&value) || WatchRtc_Write(&value) != HAL_OK)
            (void)Bluetooth_Send("ERR,VALUE\r\n");
        else
        {
            char response[24];
            (void)snprintf(response, sizeof(response), "OK,%s\r\n", field_name);
            (void)Bluetooth_Send(response);
        }
    }
}

void Bluetooth_Init(void)
{
    command_length = 0U;
    discard_command = 0U;
    last_sent_steps = StepCounter_Get();
}

void Bluetooth_Process(void)
{
    uint8_t byte;

    while (USART1_ReadByte(&byte))
    {
        if (byte == '\r')
            continue;

        if (byte == '\n')
        {
            if (discard_command != 0U)
                (void)Bluetooth_Send("ERR,LENGTH\r\n");
            else if (command_length != 0U)
                Bluetooth_HandleCommand();

            command_length = 0U;
            discard_command = 0U;
            continue;
        }

        if (discard_command != 0U)
            continue;

        if (command_length >= (BLUETOOTH_COMMAND_SIZE - 1U))
        {
            discard_command = 1U;
            continue;
        }

        command[command_length++] = (char)byte;
    }

    uint32_t steps = StepCounter_Get();
    if (steps != last_sent_steps)
    {
        char message[24];
        int length = snprintf(message, sizeof(message), "STEP,%lu\r\n",
                              (unsigned long)steps);
        if (length > 0 && (size_t)length < sizeof(message))
        {
            if (Bluetooth_Send(message) == HAL_OK)
                last_sent_steps = steps;
        }
    }
}

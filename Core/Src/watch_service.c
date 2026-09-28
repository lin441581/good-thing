#include "watch_service.h"
#include "bluetooth.h"
#include "mpu6050.h"
#include "step_counter.h"

#define STEP_SAMPLE_INTERVAL_MS 20U

static uint32_t last_step_sample;

void WatchService_Init(void)
{
    last_step_sample = HAL_GetTick();
    Bluetooth_Init();
}

void WatchService_Process(void)
{
    uint32_t now = HAL_GetTick();

    if ((now - last_step_sample) >= STEP_SAMPLE_INTERVAL_MS)
    {
        int16_t ax;
        int16_t ay;
        int16_t az;

        last_step_sample = now;
        if (MPU6050_ReadAccel(&ax, &ay, &az) == HAL_OK)
            StepCounter_Process(ax, ay, az);
    }

    Bluetooth_Process();
}

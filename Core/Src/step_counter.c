#include "step_counter.h"
#include <math.h>

static uint32_t step_count;
static int32_t previous_magnitude;
static uint32_t samples_since_step;

#define STEP_MIN_INTERVAL_SAMPLES 12U
#define STEP_ACCEL_CHANGE_THRESHOLD 1200L

void StepCounter_Init(void)
{
    step_count = 0U;
    previous_magnitude = 0;
    samples_since_step = STEP_MIN_INTERVAL_SAMPLES;
}

void StepCounter_Process(int16_t ax, int16_t ay, int16_t az)
{
    int32_t magnitude = (int32_t)sqrtf((float)ax * ax + (float)ay * ay + (float)az * az);
    int32_t change = magnitude - previous_magnitude;

    if (samples_since_step < UINT32_MAX)
        samples_since_step++;
    if (previous_magnitude != 0 && change > STEP_ACCEL_CHANGE_THRESHOLD &&
        samples_since_step >= STEP_MIN_INTERVAL_SAMPLES)
    {
        step_count++;
        samples_since_step = 0U;
    }
    previous_magnitude = (previous_magnitude * 3 + magnitude) / 4;
}

uint32_t StepCounter_Get(void)
{
    return step_count;
}

void StepCounter_Reset(void)
{
    step_count = 0U;
    previous_magnitude = 0;
    samples_since_step = STEP_MIN_INTERVAL_SAMPLES;
}

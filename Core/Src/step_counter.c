#include "step_counter.h"
#include <stdint.h>

#define STEP_GRAVITY_COUNTS 16384L
#define STEP_PEAK_THRESHOLD 3600L
#define STEP_VALLEY_THRESHOLD -1600L
#define STEP_MIN_INTERVAL 300U
#define STEP_MAX_PEAK_TO_VALLEY 800U
#define STEP_SAMPLE_INTERVAL 20U

static uint32_t StepCounter_Isqrt(uint64_t value)
{
    uint64_t bit = (uint64_t)1U << 62;
    uint64_t result = 0U;
    while (bit > value) bit >>= 2;
    while (bit != 0U)
    {
        if (value >= result + bit) { value -= result + bit; result = (result >> 1) + bit; }
        else result >>= 1;
        bit >>= 2;
    }
    return (uint32_t)result;
}

static uint32_t step_count;
static int32_t filtered_magnitude;
static int32_t gravity_baseline;
static uint32_t elapsed_ms;
static uint32_t candidate_ms;
static uint8_t waiting_for_valley;

void StepCounter_Init(void)
{
    step_count = 0U;
    filtered_magnitude = STEP_GRAVITY_COUNTS;
    gravity_baseline = STEP_GRAVITY_COUNTS;
    elapsed_ms = 0U;
    candidate_ms = 0U;
    waiting_for_valley = 0U;
}

void StepCounter_Process(int16_t ax, int16_t ay, int16_t az)
{
    int64_t square_sum = (int64_t)ax * ax + (int64_t)ay * ay + (int64_t)az * az;
    int32_t magnitude = (int32_t)StepCounter_Isqrt((uint64_t)square_sum);
    int32_t dynamic_accel;

    filtered_magnitude = (filtered_magnitude * 3 + magnitude) / 4;
    gravity_baseline += (filtered_magnitude - gravity_baseline) / 16;
    dynamic_accel = filtered_magnitude - gravity_baseline;
    elapsed_ms += STEP_SAMPLE_INTERVAL;
    if (!waiting_for_valley)
    {
        if (dynamic_accel >= STEP_PEAK_THRESHOLD)
        {
            waiting_for_valley = 1U;
            candidate_ms = 0U;
        }
        return;
    }
    candidate_ms += STEP_SAMPLE_INTERVAL;
    if (dynamic_accel <= STEP_VALLEY_THRESHOLD)
    {
        if (elapsed_ms >= STEP_MIN_INTERVAL) { step_count++; elapsed_ms = 0U; }
        waiting_for_valley = 0U;
        candidate_ms = 0U;
    }
    else if (candidate_ms >= STEP_MAX_PEAK_TO_VALLEY)
    {
        waiting_for_valley = 0U;
        candidate_ms = 0U;
    }
}

uint32_t StepCounter_Get(void)
{
    return step_count;
}

void StepCounter_Reset(void)
{
    step_count = 0U;
    elapsed_ms = STEP_MIN_INTERVAL;
    candidate_ms = 0U;
    waiting_for_valley = 0U;
}

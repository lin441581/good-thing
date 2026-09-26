#ifndef STEP_COUNTER_H
#define STEP_COUNTER_H

#include "main.h"

void StepCounter_Init(void);
void StepCounter_Process(int16_t ax, int16_t ay, int16_t az);
uint32_t StepCounter_Get(void);
void StepCounter_Reset(void);

#endif
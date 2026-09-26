#include "main.h"

#ifndef __MPU6050_H
#define __MPU6050_H

#define MPU6050_ADDR        (0x68 << 1)
#define MPU6050_WHO_AM_I    0x75
#define MPU6050_PWR_MGMT_1 0x6B

HAL_StatusTypeDef MPU6050_Init(void);
HAL_StatusTypeDef MPU6050_ReadId(uint8_t *id);
HAL_StatusTypeDef MPU6050_ReadAccel(
    int16_t *acc_x,
    int16_t *acc_y,
    int16_t *acc_z
);
#endif
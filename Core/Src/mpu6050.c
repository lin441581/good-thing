#include "mpu6050.h"
extern I2C_HandleTypeDef hi2c2;
#define MPU6050_ACCEL_XOUT_H 0x3B
HAL_StatusTypeDef MPU6050_ReadId(uint8_t *id)
{
    return HAL_I2C_Mem_Read(
        &hi2c2,
        MPU6050_ADDR,
        MPU6050_WHO_AM_I,
        I2C_MEMADD_SIZE_8BIT,
        id,
        1,
        HAL_MAX_DELAY
    );
}

#define MPU6050_ADDR          (0x68 << 1)
#define MPU6050_PWR_MGMT_1    0x6B
#define MPU6050_ACCEL_CONFIG  0x1C

extern I2C_HandleTypeDef hi2c2;

HAL_StatusTypeDef MPU6050_Init(void)
{
    HAL_StatusTypeDef status;
    uint8_t data;

    /* 唤醒 MPU6050 */
    data = 0x00;

    status = HAL_I2C_Mem_Write(
        &hi2c2,
        MPU6050_ADDR,
        MPU6050_PWR_MGMT_1,
        I2C_MEMADD_SIZE_8BIT,
        &data,
        1,
        HAL_MAX_DELAY
    );

    if (status != HAL_OK)
    {
        return status;
    }

    /* 设置加速度量程为 ±2g */
    data = 0x00;

    status = HAL_I2C_Mem_Write(
        &hi2c2,
        MPU6050_ADDR,
        MPU6050_ACCEL_CONFIG,
        I2C_MEMADD_SIZE_8BIT,
        &data,
        1,
        HAL_MAX_DELAY
    );

    return status;
}
HAL_StatusTypeDef MPU6050_ReadAccel(
    int16_t *acc_x,
    int16_t *acc_y,
    int16_t *acc_z
)
{
    uint8_t data[6];
    HAL_StatusTypeDef status;

    status = HAL_I2C_Mem_Read(
        &hi2c2,
        MPU6050_ADDR,
        MPU6050_ACCEL_XOUT_H,
        I2C_MEMADD_SIZE_8BIT,
        data,
        6,
        HAL_MAX_DELAY
    );

    if (status != HAL_OK)
    {
        return status;
    }

    *acc_x = (int16_t)((data[0] << 8) | data[1]);
    *acc_y = (int16_t)((data[2] << 8) | data[3]);
    *acc_z = (int16_t)((data[4] << 8) | data[5]);

    return HAL_OK;
}   
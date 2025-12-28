/*
 * mpu6050.c
 *
 *  Created on: Dec 28, 2025
 *      Author: wjw
 */


#include "MPU6050.h"

/* MPU6050 寄存器 */
#define MPU6050_PWR_MGMT_1     0x6B
#define MPU6050_SMPLRT_DIV     0x19
#define MPU6050_CONFIG         0x1A
#define MPU6050_GYRO_CONFIG    0x1B
#define MPU6050_ACCEL_CONFIG   0x1C
#define MPU6050_WHO_AM_I       0x75
#define MPU6050_ACCEL_XOUT_H   0x3B

void MPU6050_Init(void)
{
    uint8_t readBuffer;
    uint8_t sendBuffer[2];

    HAL_Delay(100);

    /* 1. 读取 WHO_AM_I（可选，用于调试） */
    sendBuffer[0] = MPU6050_WHO_AM_I;
    HAL_I2C_Master_Transmit(&hi2c1, MPU6050_ADDRESS, sendBuffer, 1, HAL_MAX_DELAY);
    HAL_I2C_Master_Receive(&hi2c1, MPU6050_ADDRESS, &readBuffer, 1, HAL_MAX_DELAY);
    /* readBuffer 应该为 0x68 */

    /* 2. 解除睡眠 */
    sendBuffer[0] = MPU6050_PWR_MGMT_1;
    sendBuffer[1] = 0x00;
    HAL_I2C_Master_Transmit(&hi2c1, MPU6050_ADDRESS, sendBuffer, 2, HAL_MAX_DELAY);
    HAL_Delay(10);

    /* 3. 采样率设置：1kHz / (1 + 7) = 125Hz */
    sendBuffer[0] = MPU6050_SMPLRT_DIV;
    sendBuffer[1] = 0x07;
    HAL_I2C_Master_Transmit(&hi2c1, MPU6050_ADDRESS, sendBuffer, 2, HAL_MAX_DELAY);

    /* 4. 低通滤波 */
    sendBuffer[0] = MPU6050_CONFIG;
    sendBuffer[1] = 0x06;
    HAL_I2C_Master_Transmit(&hi2c1, MPU6050_ADDRESS, sendBuffer, 2, HAL_MAX_DELAY);

    /* 5. 陀螺仪 ±2000 dps */
    sendBuffer[0] = MPU6050_GYRO_CONFIG;
    sendBuffer[1] = 0x18;
    HAL_I2C_Master_Transmit(&hi2c1, MPU6050_ADDRESS, sendBuffer, 2, HAL_MAX_DELAY);

    /* 6. 加速度 ±2g */
    sendBuffer[0] = MPU6050_ACCEL_CONFIG;
    sendBuffer[1] = 0x00;
    HAL_I2C_Master_Transmit(&hi2c1, MPU6050_ADDRESS, sendBuffer, 2, HAL_MAX_DELAY);
}

void MPU6050_Read(int16_t *ax, int16_t *ay, int16_t *az,
                  int16_t *gx, int16_t *gy, int16_t *gz,
                  int16_t *temp)
{
    uint8_t sendBuffer;
    uint8_t readBuffer[14];

    /* 指定起始寄存器 */
    sendBuffer = MPU6050_ACCEL_XOUT_H;
    HAL_I2C_Master_Transmit(&hi2c1, MPU6050_ADDRESS, &sendBuffer, 1, HAL_MAX_DELAY);

    /* 连续读取 14 字节 */
    HAL_I2C_Master_Receive(&hi2c1, MPU6050_ADDRESS, readBuffer, 14, HAL_MAX_DELAY);

    *ax = (int16_t)(readBuffer[0]  << 8 | readBuffer[1]);
    *ay = (int16_t)(readBuffer[2]  << 8 | readBuffer[3]);
    *az = (int16_t)(readBuffer[4]  << 8 | readBuffer[5]);
    *temp = (int16_t)(readBuffer[6] << 8 | readBuffer[7]);
    *gx = (int16_t)(readBuffer[8]  << 8 | readBuffer[9]);
    *gy = (int16_t)(readBuffer[10] << 8 | readBuffer[11]);
    *gz = (int16_t)(readBuffer[12] << 8 | readBuffer[13]);
}

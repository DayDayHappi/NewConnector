/*
 * mpu6050.h
 *
 *  Created on: Dec 28, 2025
 *      Author: wjw
 */

#ifndef BSP_MPU6050_MPU6050_H_
#define BSP_MPU6050_MPU6050_H_



#include "i2c.h"

/* MPU6050 I2C 地址
 * AD0 = GND -> 0x68
 * HAL 使用 8bit 地址
 */
#define MPU6050_ADDRESS  (0x68 << 1)

/* 接口函数 */
void MPU6050_Init(void);
void MPU6050_Read(int16_t *ax, int16_t *ay, int16_t *az,
                  int16_t *gx, int16_t *gy, int16_t *gz,
                  int16_t *temp);


#endif /* BSP_MPU6050_MPU6050_H_ */

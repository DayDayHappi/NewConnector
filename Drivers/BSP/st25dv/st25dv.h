/*
 * st25dv.h
 *
 *  Created on: Dec 30, 2025
 *      Author: wjw
 */

#ifndef BSP_ST25DV_ST25DV_H_
#define BSP_ST25DV_ST25DV_H_

#include "i2c.h"   // 确保能看到 hi2c1
#include "stm32f1xx_hal.h"
// ST25DV 用户区 I2C 设备地址（8bit，用于 HAL，写 A6h / 读 A7h）
#define ST25DV_I2C_ADDR_USER   (0xA6)   // 7bit=0x53，左移一位即 0xA6

// ST25 用户区采用 16 位字节地址
#define ST25_MEM_ADD_SIZE      I2C_MEMADD_SIZE_16BIT
HAL_StatusTypeDef ST25DV_WriteIPURL(void);
HAL_StatusTypeDef ST25DV_WriteCloudURL(void);

#endif /* BSP_ST25DV_ST25DV_H_ */

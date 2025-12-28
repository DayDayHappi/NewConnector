/*
 * AHT20.h
 *
 *  Created on: Dec 28, 2025
 *      Author: wjw
 */

#ifndef BSP_AHT20_AHT20_H_
#define BSP_AHT20_AHT20_H_

#include "i2c.h"
#define AHT20_ADDRESS 0x70
void AHT20_Init();
void AHT20_Read(float *Temperature, float *Humidity);

#endif /* BSP_AHT20_AHT20_H_ */

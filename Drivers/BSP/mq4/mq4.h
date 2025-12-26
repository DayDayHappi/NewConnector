/*
 * mq4.h
 *
 *  Created on: Dec 26, 2025
 *      Author: wjw
 */

#ifndef BSP_MQ4_MQ4_H_
#define BSP_MQ4_MQ4_H_

#include "stm32f1xx_hal.h"

/* ===== 用户可配置参数 ===== */
#ifndef MQ_SUPPLY_VOLT
#define MQ_SUPPLY_VOLT        5.0f
#endif

#ifndef MQ_ADC_REF
#define MQ_ADC_REF            3.3f
#endif

#ifndef MQ4_RL_KOHM
#define MQ4_RL_KOHM           10.0f
#endif

#ifndef MQ4_VDIV_RATIO
#define MQ4_VDIV_RATIO        0.3333f
#endif

#ifndef MQ4_CLEAN_AIR_RS_RO
#define MQ4_CLEAN_AIR_RS_RO   4.4f
#endif

#define MQ4_READ_TIMES        10

/* ===== MQ4 句柄结构体 ===== */
typedef struct
{
    ADC_HandleTypeDef *hadc;   // ADC 句柄（CubeMX 生成）
    uint8_t calibrated;
    float   R0;
} MQ4_HandleTypeDef;

/* ===== API ===== */
HAL_StatusTypeDef MQ4_Init(MQ4_HandleTypeDef *mq4, ADC_HandleTypeDef *hadc);
uint16_t MQ4_ReadADC_Avg(MQ4_HandleTypeDef *mq4, uint8_t times);
float MQ4_GetPPM(MQ4_HandleTypeDef *mq4);

#endif /* BSP_MQ4_MQ4_H_ */

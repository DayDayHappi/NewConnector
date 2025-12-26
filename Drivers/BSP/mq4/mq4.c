/*
 * mq4.h
 *
 *  Created on: Dec 26, 2025
 *      Author: wjw
 */


#include "./mq4/mq4.h"
#include <math.h>

HAL_StatusTypeDef MQ4_Init(MQ4_HandleTypeDef *mq4, ADC_HandleTypeDef *hadc)
{
    if (mq4 == NULL || hadc == NULL)
        return HAL_ERROR;

    mq4->hadc = hadc;
    mq4->calibrated = 0;
    mq4->R0 = 10.0f;   // 默认初值

    return HAL_OK;
}

uint16_t MQ4_ReadADC_Avg(MQ4_HandleTypeDef *mq4, uint8_t times)
{
    uint32_t sum = 0;

    for (uint8_t i = 0; i < times; i++)
    {
        HAL_ADC_Start(mq4->hadc);
        HAL_ADC_PollForConversion(mq4->hadc, 10);
        sum += HAL_ADC_GetValue(mq4->hadc);
        HAL_ADC_Stop(mq4->hadc);
        HAL_Delay(5);
    }

    return sum / times;
}

float MQ4_GetPPM(MQ4_HandleTypeDef *mq4)
{
    uint16_t adc_raw = MQ4_ReadADC_Avg(mq4, MQ4_READ_TIMES);

    float Vadc = MQ_ADC_REF * adc_raw / 4096.0f;
    if (Vadc <= 0.0f) Vadc = 0.001f;

    float Vnode = Vadc / MQ4_VDIV_RATIO;
    if (Vnode >= MQ_SUPPLY_VOLT)
        Vnode = MQ_SUPPLY_VOLT - 0.001f;

    float RS = MQ4_RL_KOHM * (MQ_SUPPLY_VOLT / Vnode - 1.0f);

    /* 首次校准 */
    if (!mq4->calibrated)
    {
        mq4->R0 = RS / MQ4_CLEAN_AIR_RS_RO;
        mq4->calibrated = 1;
    }

    /* 曲线拟合公式 */
    float ppm = powf(8.895f * mq4->R0 / RS, 1.0f / 0.637f);
    if (ppm < 0) ppm = 0;

    return ppm;
}

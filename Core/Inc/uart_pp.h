/*
 * uart_pp.h
 *
 *  Created on: Dec 30, 2025
 *      Author: wjw
 */

#ifndef INC_UART_PP_H_
#define INC_UART_PP_H_

#include "stm32f1xx_hal.h"
#include <stdint.h>

#define UART2_FRAME_LEN   15


#define UART_PP_BUF_NUM   2
#define UART_MAX_FRAME_LEN  UART2_FRAME_LEN

// ThreadFlags for Aggregator
#define FLG_U2_READY   (1U << 0)

typedef struct {
    uint8_t  buf[UART_PP_BUF_NUM][UART_MAX_FRAME_LEN]; // 双缓冲
    uint16_t frame_len;                                // 该路定长
    volatile uint8_t wr;                               // DMA 正在写的 buf index
    volatile uint8_t ready_idx;                        // 刚完成的 buf index
    volatile uint8_t ready;                            // 1=有新帧可读
    volatile uint32_t seq;                             // 帧计数
} UartPP_t;

extern UartPP_t g_u2;

// 初始化并启动四路 DMA 接收
void UartPP_StartAll(void);

// 在 HAL_UART_RxCpltCallback 内调用
void UartPP_OnRxCplt(UART_HandleTypeDef *huart);


//--------------------------------------------U1 RingBuffer------------------------------------------------------
#define U1_RING_SZ 2048
typedef struct {
  uint8_t buf[U1_RING_SZ];
  volatile uint16_t w;
  volatile uint16_t r;
} Ring_t;
extern Ring_t u1ring;
//--------------------------------------------U1 RingBuffer------------------------------------------------------

#endif /* INC_UART_PP_H_ */

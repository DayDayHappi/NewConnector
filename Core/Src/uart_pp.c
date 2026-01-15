/*
 * uart_pp.c
 *
 *  Created on: Dec 30, 2025
 *      Author: wjw
 */


#include "uart_pp.h"
#include "usart.h"      // CubeMX 生成：huart2/
#include <string.h>

UartPP_t g_u2 = {.frame_len = UART2_FRAME_LEN, .wr = 0, .ready = 0, .seq = 0};
UartPP_t g_u3 = {.frame_len = UART3_FRAME_LEN, .wr = 0, .ready = 0, .seq = 0};
volatile uint8_t Rxcp_flag = 0;
static void StartOne(UART_HandleTypeDef *huart, UartPP_t *pp)
{
    pp->wr = 0;
    pp->ready = 0;
    pp->seq = 0;
    // DMA 接收定长：直接收满 frame_len 触发 RxCpltCallback
    HAL_UART_Receive_DMA(huart, pp->buf[pp->wr], pp->frame_len);
}

void UartPP_StartAll(void)
{
    StartOne(&huart2, &g_u2);
    StartOne(&huart3, &g_u3);
}


void UartPP_OnRxCplt(UART_HandleTypeDef *huart)
{
    UartPP_t *pp = NULL;
    uint32_t flg = 0;

    if (huart->Instance == USART2) { pp = &g_u2;
    	flg = FLG_U2_READY;
    	//HAL_UART_Transmit(&huart4,"U2: ",5,500);
    }
    else if (huart->Instance == USART3) {
    		pp = &g_u3;
    		flg = FLG_U3_READY;
    		//HAL_UART_Transmit(&huart4,"U3: ",5,500);
    }
    else { return; }

    // 本次完成的 buffer = 当前 wr
//    for(int i = 0; i<20; i++) {
//    	printf("%d",(unsigned int)pp->buf[pp->ready_idx][i]);
//    }
    //HAL_UART_Transmit(&huart4,pp->buf[pp->ready_idx],20,500);
    uint8_t done = pp->wr;
    pp->ready_idx = done;
    pp->ready = 1;
    pp->seq++;

    // 切换到另一块 buffer，立刻启动下一帧 DMA
    pp->wr ^= 1U;
    HAL_UART_Receive_DMA(huart, pp->buf[pp->wr], pp->frame_len);
    Rxcp_flag = 1;
}

//--------------------------------------------U1 RingBuffer------------------------------------------------------
Ring_t u1ring;
static void Ring_Push(Ring_t *rb, const uint8_t *data, uint16_t len)
{
  for (uint16_t i=0;i<len;i++) {
    rb->buf[rb->w] = data[i];
    rb->w = (rb->w + 1) % U1_RING_SZ;
  }
}

static int Ring_PopByte(Ring_t *rb)
{
  if (rb->r == rb->w) return -1;
  uint8_t b = rb->buf[rb->r];
  rb->r = (rb->r + 1) % U1_RING_SZ;
  return b;
}
//--------------------------------------------U1 RingBuffer------------------------------------------------------

/*
 * st25dv.c
 *
 *  Created on: Dec 30, 2025
 *      Author: wjw
 */


#include "st25dv.h"
// 写入一条 URL = https://www.baidu.com 的 NDEF 记录
// 执行一次即可，EEPROM 断电也会保存
HAL_StatusTypeDef ST25DV_WriteBaiduURL(void)
{
    HAL_StatusTypeDef status;

    // 0x0000: CC + NDEF TLV 头 + NDEF 头部的一部分
    uint8_t nfc_init_page0[16] = {
        0xE2, 0x40, 0x00, 0x01, 0x00, 0x00, 0x04, 0x00, // CC (8 bytes)
        0x03, 0x0E,                                     // NDEF TLV type & length
        0xD1, 0x01, 0x0A, 0x55,                         // NDEF header
        0x02, 0x62                                      // URI prefix + 'b'
    };

    // 0x0010: 剩余 URI + Terminator
    uint8_t nfc_init_page1[16] = {
        0x61, 0x69, 0x64, 0x75, 0x2E, 0x63, 0x6F, 0x6D, // "aidu.com"
        0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00  // Terminator TLV + padding
    };

    // 写第 1 页：地址 0x0000
    status = HAL_I2C_Mem_Write(&hi2c1,
                               ST25DV_I2C_ADDR_USER,
                               0x0000,
                               ST25_MEM_ADD_SIZE,
                               nfc_init_page0,
                               sizeof(nfc_init_page0),
                               100);
    if (status != HAL_OK)
    {
        return status;
    }
    HAL_Delay(5); // 等待 EEPROM 编程完成（典型 5ms 一页）

    // 写第 2 页：地址 0x0010
    status = HAL_I2C_Mem_Write(&hi2c1,
                               ST25DV_I2C_ADDR_USER,
                               0x0010,
                               ST25_MEM_ADD_SIZE,
                               nfc_init_page1,
                               sizeof(nfc_init_page1),
                               100);
    if (status != HAL_OK)
    {
        return status;
    }
    HAL_Delay(5);

    return HAL_OK;
}
HAL_StatusTypeDef ST25DV_WriteIPURL(void)
{
    HAL_StatusTypeDef status;

    // URI = "http://120.46.133.240/"
    // 使用 URI 前缀 0x03 = "http://"
    // 后面纯 ASCII 字符： "120.46.133.240/" 一共 15 字节
    // Payload Length = 1(前缀) + 15 = 16 (0x10)
    //
    // NDEF 记录字节顺序：
    //   D1            // MB=1, ME=1, SR=1, TNF=0x1 (Well-known)
    //   01            // Type Length = 1 ('U')
    //   10            // Payload Length = 0x10 (16)
    //   55            // Type = 'U'
    //   03            // URI 前缀 0x03 = "http://"
    //   31 32 30 2E 34 36 2E 31 33 33 2E 32 34 30 2F
    //                 // "120.46.133.240/"
    //
    // NDEF 记录总长度 = 4 (头) + 16 (payload) = 20 = 0x14
    // 因此 TLV: 03 14 (type=0x03, length=0x14)

    // 0x0000: CC + TLV 头 + NDEF 头 + 前两个 payload 字节
    uint8_t nfc_init_page0[16] = {
        // CC File (8 bytes)
        0xE2, 0x40, 0x00, 0x01,
        0x00, 0x00, 0x04, 0x00,
        // NDEF TLV 头
        0x03, 0x14,             // type=0x03, length=0x14 (20字节 NDEF 记录)
        // NDEF 记录头部
        0xD1, 0x01, 0x10, 0x55, // header, type len, payload len, 'U'
        // payload 开头两字节
        0x03, 0x31              // 0x03="http://", '1'
    };

    // 0x0010: 剩余 payload + Terminator TLV
    uint8_t nfc_init_page1[16] = {
        // 剩下的 "20.46.133.240/"
        0x32, 0x30, 0x2E, 0x34, 0x36, 0x2E,
        0x31, 0x33, 0x33, 0x2E, 0x32, 0x34, 0x30, 0x2F,
        // Terminator TLV + padding
        0xFE, 0x00
    };

    // 写第 1 页：地址 0x0000
    status = HAL_I2C_Mem_Write(&hi2c1,
                               ST25DV_I2C_ADDR_USER,
                               0x0000,
                               ST25_MEM_ADD_SIZE,
                               nfc_init_page0,
                               sizeof(nfc_init_page0),
                               100);
    if (status != HAL_OK)
    {
        return status;
    }
    HAL_Delay(5); // 等待 EEPROM 编程完成

    // 写第 2 页：地址 0x0010
    status = HAL_I2C_Mem_Write(&hi2c1,
                               ST25DV_I2C_ADDR_USER,
                               0x0010,
                               ST25_MEM_ADD_SIZE,
                               nfc_init_page1,
                               sizeof(nfc_init_page1),
                               100);
    if (status != HAL_OK)
    {
        return status;
    }
    HAL_Delay(5);

    return HAL_OK;
}

HAL_StatusTypeDef ST25DV_WriteCloudURL(void)
{
    HAL_StatusTypeDef status;

    // 说明：
    // URI = "http://113.44.234.248/"
    // 前缀 0x03 = "http://"
    // 剩余 "113.44.234.248/" 共 15 字节
    // Payload Len = 1 + 15 = 16 (0x10)
    // NDEF 记录总长度 = 4(头) +16 = 20 = 0x14
    // TLV: 03 14

    // 0x0000: CC(8B) + TLV(2B) + NDEF 头(4B) + payload 前 2 字节
    uint8_t nfc_init_page0[16] = {
        // CC File (8 bytes)
        0xE2, 0x40, 0x00, 0x01,
        0x00, 0x00, 0x04, 0x00,
        // NDEF TLV 头: type=0x03, length=0x14 (20字节 NDEF)
        0x03, 0x14,
        // NDEF 头: MB=1,ME=1,SR=1,TNF=1; TypeLen=1; PayloadLen=0x10; Type='U'
        0xD1, 0x01, 0x10, 0x55,
        // Payload 开头: 0x03="http://", '1'
        0x03, 0x31
    };

    // 0x0010: 剩余 payload + Terminator TLV
    // 剩余 "13.44.234.248/" 的字节:
    // '1','3','.', '4','4','.', '2','3','4','.', '2','4','8','/'
    uint8_t nfc_init_page1[16] = {
        0x31, 0x33, 0x2E, 0x34, 0x34, 0x2E,
        0x32, 0x33, 0x34, 0x2E, 0x32, 0x34,
        0x38, 0x2F,
        0xFE, 0x00   // Terminator TLV + 填充
    };

    // 写第 1 页：0x0000
    status = HAL_I2C_Mem_Write(&hi2c1,
                               ST25DV_I2C_ADDR_USER,
                               0x0000,
                               ST25_MEM_ADD_SIZE,
                               nfc_init_page0,
                               sizeof(nfc_init_page0),
                               100);
    if (status != HAL_OK)
        return status;
    HAL_Delay(5);

    // 写第 2 页：0x0010
    status = HAL_I2C_Mem_Write(&hi2c1,
                               ST25DV_I2C_ADDR_USER,
                               0x0010,
                               ST25_MEM_ADD_SIZE,
                               nfc_init_page1,
                               sizeof(nfc_init_page1),
                               100);
    if (status != HAL_OK)
        return status;
    HAL_Delay(5);

    return HAL_OK;
}

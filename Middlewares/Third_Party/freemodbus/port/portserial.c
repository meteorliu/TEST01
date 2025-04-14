/*
 * FreeModbus Libary: BARE Port
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id$
 */

#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"


extern UART_HandleTypeDef huart1;
extern uint8_t rx_buffer[];
extern volatile uint8_t rx_len;
extern volatile uint8_t rx_idle_flag;

static uint8_t tx_buffer[256];
static uint8_t tx_len = 0;

/* ----------------------- static functions ---------------------------------*/
static void prvvUARTTxReadyISR( void );
static void prvvUARTRxISR( void );

/* ----------------------- Start implementation -----------------------------*/
void
vMBPortSerialEnable( BOOL xRxEnable, BOOL xTxEnable )
{
    /* If xRXEnable enable serial receive interrupts. If xTxENable enable
     * transmitter empty interrupts.
     */
}

BOOL
xMBPortSerialInit( UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity )
{
    // 已由 MX_UART1_Init 初始化，此处仅验证参数
    if (ucPort != 1 || ulBaudRate != 115200 || ucDataBits != 8 || eParity != MB_PAR_NONE) {
        return FALSE;
    }
    return TRUE;
}
// 使能/禁用收发
void vMBPortSerialEnable(BOOL xRxEnable, BOOL xTxEnable) {
    ENTER_CRITICAL_SECTION();
    if (xRxEnable) {
        RS485_RX_ENABLE();
        UART_Start_DMA_Rx();
    } else {
        HAL_UART_DMAStop(&huart1);
    }
    if (xTxEnable) {
        RS485_TX_ENABLE();
    } else {
        RS485_RX_ENABLE();
    }
    EXIT_CRITICAL_SECTION();
}

void vMBPortSerialEnable(BOOL xRxEnable, BOOL xTxEnable)
{
    ENTER_CRITICAL_SECTION();
    
    if (xRxEnable)
    {
        RS485_RX_ENABLE();  // 切换RS485为接收模式
        HAL_UART_Receive_DMA(&huart1, rx_buffer, RX_BUFFER_SIZE);
        __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);  // 使能空闲中断
    }
    else
    {
        HAL_UART_DMAStop(&huart1);
        __HAL_UART_DISABLE_IT(&huart1, UART_IT_IDLE);
    }

    if (xTxEnable)
    {
        RS485_TX_ENABLE();  // 切换RS485为发送模式
        __HAL_UART_ENABLE_IT(&huart1, UART_IT_TC);
    }
    else
    {
        RS485_RX_ENABLE();  // 默认切换回接收模式
        __HAL_UART_DISABLE_IT(&huart1, UART_IT_TC);
    }
    
    EXIT_CRITICAL_SECTION();
}

// 发送单个字节（缓冲到 tx_buffer）
BOOL xMBPortSerialPutByte( CHAR ucByte )
{
    if (tx_len < sizeof(tx_buffer)) {
        tx_buffer[tx_len++] = (uint8_t)ucByte;
        return TRUE;
    }
    return FALSE;
}

// 发送整个帧
void vMBPortSerialSendFrame(void) {
    HAL_UART_Transmit_DMA(&huart1, tx_buffer, tx_len);
    // 等待发送完成
    while (HAL_UART_GetState(&huart1) == HAL_UART_STATE_BUSY_TX) {
        vTaskDelay(1);
    }
    tx_len = 0;
    RS485_RX_ENABLE();
}

BOOL xMBPortSerialGetByte( CHAR * pucByte )
{
    static uint8_t rx_pos = 0;
    if (rx_idle_flag && rx_pos < rx_len) {
        *pucByte = rx_buffer[rx_pos++];
        if (rx_pos >= rx_len) {
            rx_pos = 0;
            rx_len = 0;
            rx_idle_flag = 0;
            UART_Start_DMA_Rx();
        }
        return TRUE;
    }
    return FALSE;
}

/* Create an interrupt handler for the transmit buffer empty interrupt
 * (or an equivalent) for your target processor. This function should then
 * call pxMBFrameCBTransmitterEmpty( ) which tells the protocol stack that
 * a new character can be sent. The protocol stack will then call 
 * xMBPortSerialPutByte( ) to send the character.
 */
static void prvvUARTTxReadyISR( void )
{
    pxMBFrameCBTransmitterEmpty(  );
}

/* Create an interrupt handler for the receive interrupt for your target
 * processor. This function should then call pxMBFrameCBByteReceived( ). The
 * protocol stack will then call xMBPortSerialGetByte( ) to retrieve the
 * character.
 */
static void prvvUARTRxISR( void )
{
    pxMBFrameCBByteReceived(  );
}

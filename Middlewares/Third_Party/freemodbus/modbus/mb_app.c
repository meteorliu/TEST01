#include "mb.h"

static USHORT usRegHoldingBuf[MB_REG_HOLDING_NREGS]; // 保持寄存器缓冲区（40001-40100）
static USHORT usRegInputBuf[MB_REG_INPUT_NREGS];    // 输入寄存器缓冲区（30001-30050）

/*定义线圈状态寄存器的地址起始值和存储数组*/
u8 ucRegCoilsBuf[MB_REG_COILS_SIZE / 8] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
u8 ucRegCoilsStart = MB_REG_COILS_START;

/*定义线圈输入状态寄存器的地址起始值和存储数组*/
u8 ucRegDiscreteBuf[MB_REG_DISCRETE_START] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
u8 ucRegDiscreteStart = MB_REG_DISCRETE_START;

// 初始化寄存器（可选）
void MB_RegInit(void) {
    for (int i = 0; i < MB_REG_HOLDING_NREGS; i++) {
        usRegHoldingBuf[i] = i; // 示例值
    }
    for (int i = 0; i < MB_REG_INPUT_NREGS; i++) {
        usRegInputBuf[i] = i + 1000; // 示例值
    }
}

// 保持寄存器回调
eMBErrorCode eMBRegHoldingCB(UCHAR *pucRegBuffer, USHORT usAddress, USHORT usNRegs, eMBRegisterMode eMode) {
    eMBErrorCode eStatus = MB_ENOERR;
    int iRegIndex;

    // 检查缓冲区指针
    if (pucRegBuffer == NULL) {
        return MB_EINVAL;
    }

    // 地址范围检查
    if ((usAddress >= MB_REG_HOLDING_START) && 
        (usAddress + usNRegs <= MB_REG_HOLDING_NREGS + MB_REG_HOLDING_START)) {
        iRegIndex = (int)(usAddress - MB_REG_HOLDING_START);
        
        switch (eMode) {
            case MB_REG_READ:
                while (usNRegs > 0) {
                    *pucRegBuffer++ = (UCHAR)(usRegHoldingBuf[iRegIndex] >> 8);
                    *pucRegBuffer++ = (UCHAR)(usRegHoldingBuf[iRegIndex] & 0xFF);
                    iRegIndex++;
                    usNRegs--;
                }
                break;

            case MB_REG_WRITE:
                while (usNRegs > 0) {
                    usRegHoldingBuf[iRegIndex] = *pucRegBuffer++ << 8;
                        usRegHoldingBuf[iRegIndex] |= *pucRegBuffer++;
                    iRegIndex++;
                    usNRegs--;
                }
                break;

            default:
                eStatus = MB_EINVAL;
                break;
        }
    } else {
        eStatus = MB_ENOREG;
    }
    return eStatus;
}

// 输入寄存器回调
eMBErrorCode eMBRegInputCB(UCHAR *pucRegBuffer, USHORT usAddress, USHORT usNRegs) {
    eMBErrorCode eStatus = MB_ENOERR;
    int iRegIndex;

    if ((usAddress >= MB_REG_INPUT_START) && (usAddress + usNRegs <= MB_REG_INPUT_NREGS + MB_REG_INPUT_START)) {
        iRegIndex = (int)(usAddress - MB_REG_INPUT_START);
        while (usNRegs > 0) {
            *pucRegBuffer++ = (UCHAR)(usRegInputBuf[iRegIndex] >> 8);
            *pucRegBuffer++ = (UCHAR)(usRegInputBuf[iRegIndex] & 0xFF);
            iRegIndex++;
            usNRegs--;
        }
    } else {
        eStatus = MB_ENOREG;
    }
    return eStatus;
}

// 线圈和离散输入（根据需要实现）
eMBErrorCode eMBRegCoilsCB(UCHAR *pucRegBuffer, USHORT usAddress, USHORT usNCoils, eMBRegisterMode eMode) {
    //错误状态
    eMBErrorCode eStatus = MB_ENOERR;
    //寄存器个数
    int16_t iNCoils = ( int16_t )usNCoils;
    //寄存器偏移量
    int16_t usBitOffset;

    //检查寄存器是否在指定范围内
    if( ( (int16_t)usAddress >= REG_COILS_START ) &&
            ( usAddress + usNCoils <= REG_COILS_START + REG_COILS_SIZE ) )
    {
        //计算寄存器偏移量
        usBitOffset = ( int16_t )( usAddress - REG_COILS_START );
        switch ( eMode )
        {
        //读操作
        case MB_REG_READ:
            while( iNCoils > 0 )
            {
                *pucRegBuffer++ = xMBUtilGetBits( ucRegCoilsBuf, usBitOffset,
                                                  ( uint8_t )( iNCoils > 8 ? 8 : iNCoils ) );
                iNCoils -= 8;
                usBitOffset += 8;
            }
            break;

        //写操作
        case MB_REG_WRITE:
            while( iNCoils > 0 )
            {
                xMBUtilSetBits( ucRegCoilsBuf, usBitOffset,
                                ( uint8_t )( iNCoils > 8 ? 8 : iNCoils ),
                                *pucRegBuffer++ );
                iNCoils -= 8;
                usBitOffset += 8;
            }
            break;
        }

    }
    else
    {
        eStatus = MB_ENOREG;
    }
    return eStatus;
}

eMBErrorCode eMBRegDiscreteCB(UCHAR *pucRegBuffer, USHORT usAddress, USHORT usNDiscrete) {
    //错误状态
    eMBErrorCode eStatus = MB_ENOERR;
    //操作寄存器个数
    int16_t iNDiscrete = ( int16_t )usNDiscrete;
    //偏移量
    uint16_t usBitOffset;

    //判断寄存器时候再制定范围内
    if( ( (int16_t)usAddress >= REG_DISCRETE_START ) &&
            ( usAddress + usNDiscrete <= REG_DISCRETE_START + REG_DISCRETE_SIZE ) )
    {
        //获得偏移量
        usBitOffset = ( uint16_t )( usAddress - REG_DISCRETE_START );

        while( iNDiscrete > 0 )
        {
            *pucRegBuffer++ = xMBUtilGetBits( ucRegDiscreteBuf, usBitOffset,
                                              ( uint8_t)( iNDiscrete > 8 ? 8 : iNDiscrete ) );
            iNDiscrete -= 8;
            usBitOffset += 8;
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }
    return eStatus;
}

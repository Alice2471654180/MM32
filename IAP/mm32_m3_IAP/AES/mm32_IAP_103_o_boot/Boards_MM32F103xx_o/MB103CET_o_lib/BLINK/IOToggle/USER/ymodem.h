#ifndef _YMODEM_H_
#define _YMODEM_H_
#include "hal_device.h"
/* 宏 ------------------------------------------------------------------------*/
#define PACKET_SEQNO_INDEX (1)
#define PACKET_SEQNO_COMP_INDEX (2)

#define PACKET_HEADER (3)
#define PACKET_TRAILER (2)
#define PACKET_OVERHEAD (PACKET_HEADER + PACKET_TRAILER)
#define PACKET_SIZE (128)
#define PACKET_1K_SIZE (1024)

#define FILE_NAME_LENGTH (256)
#define FILE_SIZE_LENGTH (16)

#define SOH (0x01)   //128字节数据包开始
#define STX (0x02)   //1024字节的数据包开始
#define EOT (0x04)   //结束传输
#define ACK (0x06)   //回应
#define NAK (0x15)   //没回应
#define CA (0x18)    //这两个相继中止转移
#define CRC16 (0x43) //'C' == 0x43, 需要 16-bit CRC

#define ABORT1 (0x41) //'A' == 0x41, 用户终止
#define ABORT2 (0x61) //'a' == 0x61, 用户终止

#define NAK_TIMEOUT (0x100000)
#define MAX_ERRORS (5)


static uint8_t Tab_1K[1024] = {0};
#define APPLICATION_ADDRESS     (uint32_t)0x08003000
#define IAP_UART  UART1   /* Used for IAP */
static uint8_t FileName[FILE_NAME_LENGTH];
#define USER_FLASH_SIZE   (USER_FLASH_END_ADDRESS - APPLICATION_ADDRESS + 1)
/* 函数声明 ------------------------------------------------------------------*/
int32_t Ymodem_Receive(uint8_t *);
uint8_t Ymodem_Transmit(uint8_t *, const uint8_t *, uint32_t);
int32_t Ymodem_Download(uint8_t *Buf);
#endif /* _YMODEM_H_ */

/*******************************文件结束***************************************/

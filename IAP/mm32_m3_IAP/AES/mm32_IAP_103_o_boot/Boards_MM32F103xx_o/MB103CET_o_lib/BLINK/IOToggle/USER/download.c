#include "common.h"

/* �������� ------------------------------------------------------------------*/
extern uint8_t file_name[FILE_NAME_LENGTH];
uint8_t tab_1024[1024] =
{
    0
};

/*******************************************************************************
  * @��������   SerialDownload
  * @����˵��   ͨ�����ڽ���һ���ļ�
  * @�������   ��
  * @�������   ��
  * @���ز���   ��
*******************************************************************************/
void SerialDownload(void)
{
    uint8_t Number[10] = "          ";
    int32_t Size = 0;

    SerialPutString("Waiting for the file(press 'a' to abort)\r\n");
    Size = Ymodem_Receive(&tab_1024[0]);
    if (Size > 0)
    {
        SerialPutString("\r\nProgramming Successfully!\r\nName: ");
        SerialPutString(file_name);
        Int2Str(Number, Size);
        SerialPutString("\r\nSize: ");
        SerialPutString(Number);
        SerialPutString("Bytes\r\n");
    }
    else if (Size == -1)
    {
        SerialPutString("\r\nThe image size is higher than the allowed space memory!\r\n");
    }
    else if (Size == -2)
    {
        SerialPutString("\r\n\rVerification failed!\r\n");
    }
    else if (Size == -3)
    {
        SerialPutString("\r\nAborted by user.\r\n");
    }
    else
    {
        SerialPutString("\r\nFailed to receive the file!\r\n");
    }
}

/*******************************�ļ�����***************************************/

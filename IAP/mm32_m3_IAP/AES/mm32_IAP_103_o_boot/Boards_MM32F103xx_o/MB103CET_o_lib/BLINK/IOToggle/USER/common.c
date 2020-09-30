#include "common.h"
//#include "ymodem.h"

/* ���� ----------------------------------------------------------------------*/
pFunction Jump_To_Application;
uint32_t JumpAddress;
uint32_t BlockNbr = 0, UserMemoryMask = 0;
__IO uint32_t FlashProtection = 0;
extern uint32_t FlashDestination;

/*******************************************************************************
  * @��������   Int2Str
  * @����˵��   ��������ת���ַ���
  * @�������   intnum:����
  * @�������   str��ת��Ϊ���ַ���
  * @���ز���   ��
*******************************************************************************/
void Int2Str(uint8_t *str, int32_t intnum)
{
    uint32_t i, Div = 1000000000, j = 0, Status = 0;

    for (i = 0; i < 10; i++)
    {
        str[j++] = (intnum / Div) + 48;

        intnum = intnum % Div;
        Div /= 10;
        if ((str[j - 1] == '0') & (Status == 0))
        {
            j = 0;
        }
        else
        {
            Status++;
        }
    }
}

/*******************************************************************************
  * @��������   Int2Str
  * @����˵��   �ַ���ת������
  * @�������   inputstr:��ת�����ַ���
  * @�������   intnum��ת�õ�����
  * @���ز���   ת�����
                1����ȷ
                0������
*******************************************************************************/
uint32_t Str2Int(uint8_t *inputstr, int32_t *intnum)
{
    uint32_t i = 0, res = 0;
    uint32_t val = 0;

    if (inputstr[0] == '0' && (inputstr[1] == 'x' || inputstr[1] == 'X'))
    {
        if (inputstr[2] == '\0')
        {
            return 0;
        }
        for (i = 2; i < 11; i++)
        {
            if (inputstr[i] == '\0')
            {
                *intnum = val;
                //����1
                res = 1;
                break;
            }
            if (ISVALIDHEX(inputstr[i]))
            {
                val = (val << 4) + CONVERTHEX(inputstr[i]);
            }
            else
            {
                //��Ч���뷵��0
                res = 0;
                break;
            }
        }

        if (i >= 11)
        {
            res = 0;
        }
    }
    else //���10Ϊ2����
    {
        for (i = 0; i < 11; i++)
        {
            if (inputstr[i] == '\0')
            {
                *intnum = val;
                //����1
                res = 1;
                break;
            }
            else if ((inputstr[i] == 'k' || inputstr[i] == 'K') && (i > 0))
            {
                val = val << 10;
                *intnum = val;
                res = 1;
                break;
            }
            else if ((inputstr[i] == 'm' || inputstr[i] == 'M') && (i > 0))
            {
                val = val << 20;
                *intnum = val;
                res = 1;
                break;
            }
            else if (ISVALIDDEC(inputstr[i]))
            {
                val = val * 10 + CONVERTDEC(inputstr[i]);
            }
            else
            {
                //��Ч���뷵��0
                res = 0;
                break;
            }
        }
        //����10λ��Ч������0
        if (i >= 11)
        {
            res = 0;
        }
    }

    return res;
}

/*******************************************************************************
  * @��������   Int2Str
  * @����˵��   �ַ���ת������
  * @�������   inputstr:��ת�����ַ���
  * @�������   intnum��ת�õ�����
  * @���ز���   ת�����
                1����ȷ
                0������
*******************************************************************************/
/**
  * @brief  Get an integer from the HyperTerminal
  * @param  num: The inetger
  * @retval 1: Correct
  *         0: Error
  */
uint32_t GetIntegerInput(int32_t *num)
{
    uint8_t inputstr[16];

    while (1)
    {
        GetInputString(inputstr);
        if (inputstr[0] == '\0')
            continue;
        if ((inputstr[0] == 'a' || inputstr[0] == 'A') && inputstr[1] == '\0')
        {
            SerialPutString("User Cancelled \r\n");
            return 0;
        }

        if (Str2Int(inputstr, num) == 0)
        {
            SerialPutString("Error, Input again: \r\n");
        }
        else
        {
            return 1;
        }
    }
}

/*******************************************************************************
  * @��������   SerialKeyPressed
  * @����˵��   ���Գ����ն��Ƿ��а�������
  * @�������   key:����
  * @�������   ��
  * @���ز���   1����ȷ
                0������
*******************************************************************************/
uint32_t SerialKeyPressed(uint8_t *key)
{

    if (UART_GetFlagStatus(UART1, UART_FLAG_RXAVL) != RESET)
    {
        *key = (uint8_t)UART1->RDR;
        return 1;
    }
    else
    {
        return 0;
    }
}

/*******************************************************************************
  * @��������   GetKey
  * @����˵��   ͨ�������жϻ�ȥ����
  * @�������   ��
  * @�������   ��
  * @���ز���   ���µļ���
*******************************************************************************/
uint8_t GetKey(void)
{
    uint8_t key = 0;

    //�ȴ���������
    while (1)
    {
        if (SerialKeyPressed((uint8_t *)&key))
            break;
    }
    return key;
}

/*******************************************************************************
  * @��������   SerialPutChar
  * @����˵��   ���ڷ���һ���ַ�
  * @�������   C:�跢�͵��ַ�
  * @�������   i��
  * @���ز���   ��
*******************************************************************************/
void SerialPutChar(uint8_t c)
{
    UART_SendData(UART1, c);
    while (UART_GetFlagStatus(UART1, UART_FLAG_TXEMPTY) == RESET)
    {
    }
}

/*******************************************************************************
  * @��������   SerialPutChar
  * @����˵��   ���ڷ���һ���ַ���
  * @�������   *s:�跢�͵��ַ���3
  * @�������   ��
  * @���ز���   ��
*******************************************************************************/
void Serial_PutString(uint8_t *s)
{
    while (*s != '\0')
    {
        SerialPutChar(*s);
        s++;
    }
}

/*******************************************************************************
  * @��������   GetInputString
  * @����˵��   �Ӵ��ڻ�ȡһ���ַ���
  * @�������   *s:���ַ����ĵ�ַ
  * @�������   ��
  * @���ز���   ��
*******************************************************************************/
void GetInputString(uint8_t *buffP)
{
    uint32_t bytes_read = 0;
    uint8_t c = 0;
    do
    {
        c = GetKey();
        if (c == '\r')
            break;
        if (c == '\b') // Backspace ����
        {
            if (bytes_read > 0)
            {
                SerialPutString("\b \b");
                bytes_read--;
            }
            continue;
        }
        if (bytes_read >= CMD_STRING_SIZE)
        {
            SerialPutString("Command string size overflow\r\n");
            bytes_read = 0;
            continue;
        }
        if (c >= 0x20 && c <= 0x7E)
        {
            buffP[bytes_read++] = c;
            SerialPutChar(c);
        }
    }
    while (1);
    SerialPutString(("\n\r"));
    buffP[bytes_read] = '\0';
}

/*******************************************************************************
  * @��������   FLASH_PagesMask
  * @����˵��   ����Falshҳ
  * @�������   Size:�ļ�����
  * @�������   ��
  * @���ز���   ҳ������
*******************************************************************************/
uint32_t FLASH_PagesMask(__IO uint32_t Size)
{
    uint32_t pagenumber = 0x0;
    uint32_t size = Size;

    if ((size % PAGE_SIZE) != 0)
    {
        pagenumber = (size / PAGE_SIZE) + 1;
    }
    else
    {
        pagenumber = size / PAGE_SIZE;
    }
    return pagenumber;
}

/*******************************************************************************
  * @��������   FLASH_DisableWriteProtectionPages
  * @����˵��   �Ӵ�Flashд����
  * @�������   ��
  * @�������   ��
  * @���ز���   ��
*******************************************************************************/
void FLASH_DisableWriteProtectionPages(void)
{
    uint32_t useroptionbyte = 0, WRPR = 0;
    uint16_t var1 = OB_IWDG_SW, var2 = OB_STOP_NoRST, var3 = OB_STDBY_NoRST;
    FLASH_Status status = FLASH_BUSY;

    WRPR = FLASH_GetWriteProtectionOptionByte();

    //�����Ƿ�д����
    if ((WRPR & UserMemoryMask) != UserMemoryMask)
    {
        useroptionbyte = FLASH_GetUserOptionByte();

        UserMemoryMask |= WRPR;

        status = FLASH_EraseOptionBytes();

        if (UserMemoryMask != 0xFFFFFFFF)
        {
            status = FLASH_EnableWriteProtection((uint32_t)~UserMemoryMask);
        }
        //�ô�ѡ�����Ƿ��б��
        if ((useroptionbyte & 0x07) != 0x07)
        {
            //���±���ѡ����
            if ((useroptionbyte & 0x01) == 0x0)
            {
                var1 = OB_IWDG_HW;
            }
            if ((useroptionbyte & 0x02) == 0x0)
            {
                var2 = OB_STOP_RST;
            }
            if ((useroptionbyte & 0x04) == 0x0)
            {
                var3 = OB_STDBY_RST;
            }

            FLASH_UserOptionByteConfig(var1, var2, var3);
        }

        if (status == FLASH_COMPLETE)
        {
            SerialPutString("Write Protection disabled...\r\n");

            SerialPutString("...and a System Reset will be generated to re-load the new option bytes\r\n");
            //ϵͳ��λ���¼���ѡ����
            NVIC_SystemReset();
        }
        else
        {
            SerialPutString("Error: Flash write unprotection failed...\r\n");
        }
    }
    else
    {
        SerialPutString("Flash memory not write protected\r\n");
    }
}

/*******************************************************************************
  * @��������   Main_Menu
  * @����˵��   ��ʾ�˵����ڳ����ն�
  * @�������   ��
  * @�������   ��
  * @���ز���   ��
*******************************************************************************/
void Main_Menu(void)
{
    uint8_t key = 0;
    /* Get the number of block (4 or 2 pages) from where the user program will be loaded */
    /*���û�����Ҫ���صĵط���ȡ�������(4��2ҳ)*/
    BlockNbr = (FlashDestination - DefaultStartAddress) >> 12;//FLASH��ҳ��飬ÿҳΪ1/2k,ÿ��4k
    /* Compute the mask to test if the Flash memory, where the user program will be
        loaded, is write protected */
    if (BlockNbr < 62)
    {
        UserMemoryMask = ((uint32_t) ~((1 << BlockNbr) - 1));//�˴�������ȡ�����ֵ
    }
    else
    {
        UserMemoryMask = ((uint32_t)DefaultStartAddress);//�˴�����
    }
//����FLASHд����ѡ���ֽڵ�ֵFLASH_GetWriteProtectionOptionByte()
    if ((FLASH_GetWriteProtectionOptionByte() & UserMemoryMask) != UserMemoryMask)
        //�����д����Ϊ0���ж��������ֵ&д�����Ĵ�������ֵ�Ƿ�����Լ����ж��Ƿ���Խ�������
    {
        FlashProtection = 1;
    }
    else
    {
        FlashProtection = 0;
    }

    while (1)
    {
        SerialPutString("\r\n=========== Main Menu ==========\r\n");
        SerialPutString("= Download Image To Flash -- 1 =\r\n");
        SerialPutString("= Execute The New Program -- 2 =\r\n");

        if (FlashProtection != 0)
        {
            SerialPutString("Disable the write protection -- 3\r\n");
        }
        SerialPutString("================================\r\n");
        key = GetKey();

        if (key == 0x31)
        {
            //���س���
            SerialDownload();
        }
        else if (key == 0x32)
        {
						SerialPutString("Execute user 11111\r\n");
            SerialPutString("Execute user Program\r\n");
            JumpAddress = *(__IO uint32_t *)(ApplicationAddress + 4);

            //��ת���û�����
            Jump_To_Application = (pFunction)JumpAddress;
            //��ʼ���û�����Ķ�ջָ��
            __set_MSP(*(__IO uint32_t *)ApplicationAddress);
            Jump_To_Application();
        }
        else if ((key == 0x33) && (FlashProtection == 1))
        {
            //���д����
            FLASH_DisableWriteProtectionPages();
        }
        else if (key == 'r')
        {
            SerialPutString("\r\nSystem Reboot...\r\n");
            __set_FAULTMASK(1);
            NVIC_SystemReset();
        }
        else
        {
            if (FlashProtection == 0)
            {
                SerialPutString("The number should be either 1 or 2\r\n");
            }
            else
            {
                SerialPutString("The number should be either 1, 2 or 3\r\n");
            }
        }
    }
}

/*******************************�ļ�����***************************************/

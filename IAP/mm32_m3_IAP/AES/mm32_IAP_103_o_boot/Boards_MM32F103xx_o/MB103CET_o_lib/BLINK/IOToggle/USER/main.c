#include "HAL_conf.h"
#include "HAL_device.h"
#include "common.h"
unsigned char UpdateFlag __attribute__((at(0x801FFFC)));//�����ľ��Զ�λ
/* ���� ----------------------------------------------------------------------*/
extern pFunction Jump_To_Application;
extern uint32_t JumpAddress;
/* �������� ------------------------------------------------------------------*/
void Delay(__IO uint32_t nCount);
void LED_Configuration(void);
static void IAP_Init(void);
void KEY_Configuration(void);
void GPIO_Configuration(void);
void USART_Configuration(void);
/* �������� ------------------------------------------------------------------*/

/*******************************************************************************
  * @��������   main
  * @����˵��   ������
  * @�������   ��
  * @�������   ��
  * @���ز���   ��
*******************************************************************************/

int main(void)
{
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0000);//���������ж�������//0x0000Ϊ�������ƫ�Ƶ�ַ
    FLASH_Unlock(); //Flash ����
    IAP_Init();//���ڳ�ʼ������
    //��⵽���±��
    if (!UpdateFlag)
    {
        //ִ��IAP�����������Flash����
        Main_Menu();
    }
    //����ִ���û�����
    else
    {
        //�ж��û��Ƿ��Ѿ����س�����Ϊ��������´˵�ַ��ջ��ַ��
        //��û����һ��Ļ�����ʹû�����س���Ҳ�����������ܷɡ�
        if (((*(__IO uint32_t *)ApplicationAddress) & 0x2FFE0000) == 0x20000000)//�ж��Ƿ��������Ҳ����д��(((*(__IO uint32_t *)ApplicationAddress) ��=0xffffffff)
        //if (!((*(__IO uint32_t *)ApplicationAddress) ==0xFFFFFFFF))
			{
				SerialPutString("Execute user Program11111\r\n");
            SerialPutString("Execute user Program\r\n");
            //��ת���û�����
            JumpAddress = *(__IO uint32_t *)(ApplicationAddress + 4);
            Jump_To_Application = (pFunction)JumpAddress;
						__disable_irq();
						UART_ITConfig(UART1 ,UART_IT_RXIEN,DISABLE),
            //��ʼ���û�����Ķ�ջָ��
            __set_MSP(*(__IO uint32_t *)ApplicationAddress);

            Jump_To_Application();
        }
        else
        {
            SerialPutString("no user Program\r\n");
            Main_Menu();
        }
    }

    while (1)
    {
    }
}

/*******************************************************************************
  * @��������   GPIO_Configuration
  * @����˵��   ����ʹ��USART1�����IO�ܽ�
  * @�������   ��
  * @�������   ��
  * @���ز���   ��
*******************************************************************************/
void GPIO_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    /*�����������*/
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_7);

    /*UART1_RX  GPIOA.10��ʼ��*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    /*��������*/
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_7);
}

/*******************************************************************************
  * @��������   IAP_Init
  * @����˵��   ����ʹ��IAP
  * @�������   ��
  * @�������   ��
  * @���ز���   ��
*******************************************************************************/
void IAP_Init(void)
{
    UART_InitTypeDef UART_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_UART1, ENABLE);
    /* USART1 ���� ------------------------------------------------------------
           USART1 ��������:
            - ������      = 115200 baud
            - �ֳ�        = 8 Bits
            - һ��ֹͣλ
            - ��У��
            - ��Ӳ��������
            - ���ܺͷ���ʹ��
      --------------------------------------------------------------------------*/
    UART_InitStructure.UART_BaudRate = 19200;
    /*�ֳ�Ϊ8λ���ݸ�ʽ*/
    UART_InitStructure.UART_WordLength = UART_WordLength_8b;
    /*һ��ֹͣλ*/
    UART_InitStructure.UART_StopBits = UART_StopBits_1;
    /*����żУ��λ*/
    UART_InitStructure.UART_Parity = UART_Parity_No;
    /*��Ӳ������������*/
    UART_InitStructure.UART_HardwareFlowControl = UART_HardwareFlowControl_None;
    /*�շ�ģʽ*/
    UART_InitStructure.UART_Mode = UART_Mode_Rx | UART_Mode_Tx;
    /*��ʼ������1*/
    UART_Init(UART1, &UART_InitStructure);
    /*�������ڽ����ж�*/
    /*ʹ�ܴ���1*/
    GPIO_Configuration();
    // ʹ�� USART3
    UART_Cmd(UART1, ENABLE);
}

/*******************************************************************************
  * @��������   Delay
  * @����˵��   ����һ����ʱʱ��
  * @�������   nCount: ָ����ʱʱ�䳤��
  * @�������   ��
  * @���ز���   ��
*******************************************************************************/
void Delay(__IO uint32_t nCount)
{
    for (; nCount != 0; nCount--)
        ;
}



void UARTx_WriteByte(UART_TypeDef *UARTx,uint8_t Data)
{ 
    /* send a character to the UART */
    UART_SendData(UARTx, Data);

     /* Loop until the end of transmission */
    while(UART_GetFlagStatus(UARTx, UART_FLAG_TXEPT) == RESET);
}


#ifdef USE_FULL_ASSERT

/*******************************************************************************
  * @��������   assert_failed
  * @����˵��   �����ڼ�������������ʱ��Դ�ļ����ʹ�������
  * @�������   file: Դ�ļ���
                line: ������������
  * @�������   ��
  * @���ز���   ��
*******************************************************************************/
void assert_failed(uint8_t *file, uint32_t line)
{
    /* �û����������Լ��Ĵ������ڱ��������ļ�������������,
         ���磺printf("�������ֵ: �ļ��� %s �� %d��\r\n", file, line) */

    //��ѭ��
    while (1)
    {
    }
}
#endif



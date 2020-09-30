#include "HAL_conf.h"
#include "HAL_device.h"
#include "stdio.h"
#include "string.h"
void Tim2_UPCount_test(u16 Prescaler, u16 Period);

unsigned char UpdateFlag __attribute__((at(0x801FFFC)));
void IAP_Updata(void);


void ClearBootFlag(void)
{
    FLASH_Status FLASHStatus = FLASH_COMPLETE;
    if (!UpdateFlag)
    {
        //��������־λ
        printf("�������IAP������־...\r\n\r\n");
        FLASH_Unlock();
        FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
        FLASHStatus = FLASH_ErasePage((uint32_t)&UpdateFlag);
        while (FLASHStatus != FLASH_COMPLETE)
            ;
        FLASH_Lock();
    }
}

void RestartForBoot(void)
{
    FLASH_Status FLASHStatus = FLASH_COMPLETE;

    if (UpdateFlag)
    {
        //д������־λ
        FLASH_Unlock();
        FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
        FLASHStatus = FLASH_ProgramWord((uint32_t)&UpdateFlag, 0x00);
        while (FLASHStatus != FLASH_COMPLETE)
            ;
        FLASH_Lock();
        printf("������־λд��ɹ�...\r\n\r\n");
    }

    printf("��������IAP��������...\r\n\r\n");
    //��ת��IAP
    __set_FAULTMASK(1);
    NVIC_SystemReset();
}



char  uart_flag = 0;
u8 updata_cmd[6] = {0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};
u8 upadte_flag = 0;
/*�����������ֽ��� 200*/
#define UART_REC_LEN            200
/*ʹ�ܣ�1��/��ֹ��0������1����*/
#define EN_UART1_RX             1

#define LED4_ON()  GPIO_ResetBits(GPIOB,GPIO_Pin_5)
#define LED4_OFF()  GPIO_SetBits(GPIOB,GPIO_Pin_5)
#define LED4_TOGGLE()  (GPIO_ReadOutputDataBit(GPIOB,GPIO_Pin_5))?(GPIO_ResetBits(GPIOB,GPIO_Pin_5)):(GPIO_SetBits(GPIOB,GPIO_Pin_5))

#define LED3_ON()  GPIO_ResetBits(GPIOB,GPIO_Pin_4)
#define LED3_OFF()  GPIO_SetBits(GPIOB,GPIO_Pin_4)
#define LED3_TOGGLE()  (GPIO_ReadOutputDataBit(GPIOB,GPIO_Pin_4))?(GPIO_ResetBits(GPIOB,GPIO_Pin_4)):(GPIO_SetBits(GPIOB,GPIO_Pin_4))

#define LED1_ON()  GPIO_ResetBits(GPIOA,GPIO_Pin_15)
#define LED1_OFF()  GPIO_SetBits(GPIOA,GPIO_Pin_15)
#define LED1_TOGGLE()  (GPIO_ReadOutputDataBit(GPIOA,GPIO_Pin_15))?(GPIO_ResetBits(GPIOA,GPIO_Pin_15)):(GPIO_SetBits(GPIOA,GPIO_Pin_15))



		u8 t=0;
    u8 len=0;
    u16 times = 0;



/*����״̬���*/
u16 UART_RX_STA = 0;
/*���ջ���,���UART_REC_LEN���ֽ�*/
u8 UART_RX_BUF[UART_REC_LEN];
char printBuf[100];
/*us��ʱ������*/
static u8  fac_us = 0;
/*ms��ʱ������,��ucos��,����ÿ�����ĵ�ms��*/
static u16 fac_ms = 0;
void UART_Send_Message(u8 *Data);
void delay_init(void);
void delay_ms(u16 nms);
void UartSendGroup(u8 *buf, u16 len);
void uart_nvic_init(u32 bound);
void LED_Init(void);
void UartSendGroup(u8 *buf, u16 len);

int fputc(int ch, FILE *f)
{
    while ((UART1->CSR & UART_IT_TXIEN) == 0); //ѭ������,ֱ���������
    UART1->TDR = (ch & (uint16_t)0x00FF);
    return ch;
}

/********************************************************************************************************
**������Ϣ ��int main (void)
**�������� �������󣬴��ڷ������ݣ����ȴ���λ��������Ϣ��Ȼ���ٴ�ӡ����
**������� ��
**������� ��
********************************************************************************************************/
int main(void)
{
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x3000);

		__enable_irq();
	
    delay_init();
    LED_Init();

    uart_nvic_init(19200);
		Tim2_UPCount_test(SystemCoreClock / 1000000 - 1, 1000 - 1);
    ClearBootFlag();
		printf("MM32_TEST\r\n");
		UartSendGroup((u8*)printBuf, sprintf(printBuf,"\r\n�����͵���ϢΪ:te\r\n"));
    while (1)
    {
			LED1_TOGGLE();
			delay_ms(1000);
		}
}

void UART_Send_Message(u8 *Data)
{
    while (*Data != '\0')
    {
        UART_SendData(UART1, *Data);
        while (UART_GetFlagStatus(UART1, UART_IT_TXIEN) == RESET); //��ȡ����״̬
        Data++;
    }
}

/********************************************************************************************************
**������Ϣ ��void uart_nvic_init(u32 bound)
**�������� ��UART�˿ڡ��жϳ�ʼ��
**������� ��bound
**������� ��
**    ��ע ��
********************************************************************************************************/
void uart_nvic_init(u32 bound)
{
    /*GPIO�˿�����*/
    GPIO_InitTypeDef GPIO_InitStructure;
    UART_InitTypeDef UART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    /*ʹ��UART1��GPIOAʱ��*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_UART1, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

    /*UART1 NVIC ����*/
    NVIC_InitStructure.NVIC_IRQChannel = UART1_IRQn;
    /*��ռ���ȼ�3*/
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3 ;
    /*�����ȼ�3*/
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    /*IRQͨ��ʹ��*/
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    /*����ָ���Ĳ�����ʼ��VIC�Ĵ���*/
    NVIC_Init(&NVIC_InitStructure);

    /*UART ��ʼ������*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_UART1, ENABLE);



		UART_DeInit(UART1);

    /*���ڲ�����*/
    UART_InitStructure.UART_BaudRate = bound;
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
    UART_ITConfig(UART1, UART_IT_RXIEN, ENABLE);
    /*ʹ�ܴ���1*/
    UART_Cmd(UART1, ENABLE);

    /*UART1_TX   GPIOA.9*/
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


/********************************************************************************************************
**������Ϣ ��void UART1_IRQHandler(void)
**�������� ������1�жϷ������
**������� ��
**������� ��
**    ��ע ��
********************************************************************************************************/
/***************************test1****************************/
//void UART1_IRQHandler(void)
//{

//    u8 Res;
//    /*�����ж�(���յ������ݱ�����0x0d 0x0a��β)*/
//    if (UART_GetITStatus(UART1, UART_IT_RXIEN)  != RESET)
//    {

//        UART_ClearITPendingBit(UART1, UART_IT_RXIEN);
//        /*��ȡ���յ�������*/
//        Res = UART_ReceiveData(UART1);
//        if ((Res == 0x0d) && ((UART_RX_STA & 0X3FFF) > 0))
//        {
//            UART_RX_STA |= 0x4000;
//            UART_RX_BUF[UART_RX_STA & 0X3FFF] = Res ;
//            UART_RX_STA++;
//        }
//        /*���յ���0x0d*/
//        else if ((UART_RX_STA & 0x4000) && ((UART_RX_STA & 0X3FFF) > 0))
//        {
//            if (Res == 0x0a)
//            {
//                UART_RX_STA |= 0x8000;
//            }
//            UART_RX_BUF[UART_RX_STA & 0X3FFF] = Res ;
//            UART_RX_STA++;
//            IAP_Updata();
//        }
//        else
//        {
//            UART_RX_BUF[UART_RX_STA & 0X3FFF] = Res ;
//            UART_RX_STA++;
//            UART_RX_STA = UART_RX_STA & 0X3FFF;
//            if ((UART_RX_STA) > (UART_REC_LEN - 1))
//                /*�������ݴ���,���¿�ʼ����*/
//                UART_RX_STA = 0;
//        }
//    }
//}


u8  UART_IDLE_start=0;

void UART1_IRQHandler(void)                	//����1�жϷ������
{
    u8 Res;
	
    if(UART_GetITStatus(UART1, UART_IT_RXIEN)  != RESET)  //�����ж�(ʹ�ö�ʱ����ʱ�趨��ʱ�ж�ģ��UART�����ж�)
    {
        UART_ClearITPendingBit(UART1, UART_IT_RXIEN);
        Res = UART_ReceiveData(UART1);	//��ȡ���յ�������			  
				UART_RX_BUF[UART_RX_STA & 0X3FFF] = Res ;
				UART_RX_STA++;
				UART_RX_STA = UART_RX_STA & 0X3FFF;
				if((UART_RX_STA) > (UART_REC_LEN - 1))
				{
						UART_RX_STA = 0; //�������ݴ���,���¿�ʼ����
					  UART_IDLE_start=0;
				}        
        //=================================================
				TIM2->CNT=0;          //ÿ�յ�һ���ֽ����������㶨ʱ��������ʹ��ʱ���������¼���
				if(UART_IDLE_start==0)//UART���յ�����֡��һ���ֽ���ʹ�ܶ�ʱ��
				{
				  UART_IDLE_start=1;
					TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
					TIM_Cmd(TIM2,ENABLE); 	//ʹ�ܶ�ʱ��2 ��ʼ����UART���м�ʱ
				}
				
    }
}
char  data_length=0;
void TIM2_IRQHandler(void)
{
    /*�����ֶ�����жϱ�־λ*/
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    
	  //=========================================
    UART_RX_STA |= 0x8000;  //��ʾģ������ж��Ѳ���
		UART_IDLE_start=0;      //����1֡���������ݽ��պ����IDLE��ʼ��־λ
		TIM_Cmd(TIM2,DISABLE); 	//�رն�ʱ��2	
		printf("DATA receive over\r\n");
		data_length=UART_RX_STA&0x7fff;
		UART_RX_STA=0;
		IAP_Updata();
	
}

/********************************************************************************************************
**������Ϣ ��void Tim2_UPCount_test1(u16 Period,u16 Prescaler)
**�������� �����ö�ʱ��2���ϼ���ģʽ
**������� ��Period 16λ����������ֵ
Prescaler ʱ��Ԥ��Ƶֵ
**������� ����
********************************************************************************************************/
void Tim2_UPCount_test(u16 Prescaler, u16 Period)
{
    TIM_TimeBaseInitTypeDef TIM_StructInit;
    NVIC_InitTypeDef NVIC_StructInit;

    /*ʹ��TIM1ʱ��,Ĭ��ʱ��ԴΪPCLK1(PCLK1δ��Ƶʱ����Ƶ,������PCLK1��Ƶ���),��ѡ����ʱ��Դ*/
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);


    TIM_StructInit.TIM_Period = Period;                                                                    //ARR�Ĵ���ֵ
    TIM_StructInit.TIM_Prescaler = Prescaler;                                                              //Ԥ��Ƶֵ
    /*�����˲�������Ƶ��,��Ӱ�춨ʱ��ʱ��*/
    TIM_StructInit.TIM_ClockDivision = TIM_CKD_DIV1;                                                       //������Ƶֵ
    TIM_StructInit.TIM_CounterMode = TIM_CounterMode_Up;                                                   //����ģʽ
    TIM_StructInit.TIM_RepetitionCounter = 0;

    TIM_TimeBaseInit(TIM2, &TIM_StructInit);
	
    /*���¶�ʱ��ʱ���������ʱ��,�����־λ*/
    TIM_ClearFlag(TIM2, TIM_FLAG_Update);
	
    /*����ʱ��2�����ж�*/
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);



    /* ���ö�ʱ��2�ж�ͨ�������ȼ� */
    NVIC_StructInit.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_StructInit.NVIC_IRQChannelCmd = ENABLE;
    NVIC_StructInit.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_StructInit.NVIC_IRQChannelSubPriority = 1;

    NVIC_Init(&NVIC_StructInit);
		
		//TIM_Cmd(TIM2, ENABLE);
}







/********************************************************************************************************
**������Ϣ ��LED_Init(void)
**�������� ��LED��ʼ��
**������� ����
**������� ����
********************************************************************************************************/
void LED_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_4 | GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    LED1_OFF();
    LED3_OFF();
    LED4_OFF();

    LED1_ON();
    LED3_ON();
    LED4_ON();
}

/********************************************************************************************************
**������Ϣ ��void delay_init(void)
**�������� ����ʼ���ӳٺ���
**������� ��
**������� ��
**���ú��� ��
********************************************************************************************************/
void delay_init(void)
{
    /*ѡ���ⲿʱ��  HCLK/8*/
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
    /*Ϊϵͳʱ�ӵ�1/8*/
    fac_us = SystemCoreClock / 8000000;
    /*��OS��,����ÿ��ms��Ҫ��systickʱ���� */
    fac_ms = (u16)fac_us * 1000;
}


/********************************************************************************************************
**������Ϣ ��void delay_ms(u16 nms)
**�������� ����ʱnms
**������� ��nms
**������� ��
**    ��ע ��SysTick->LOADΪ24λ�Ĵ���,����,�����ʱΪ:nms<=0xffffff*8*1000/SYSCLK,72M������,nms<=1864
********************************************************************************************************/
void delay_ms(u16 nms)
{
    u32 temp;
    /*ʱ�����(SysTick->LOADΪ24bit)*/
    SysTick->LOAD = (u32)nms * fac_ms;
    /*��ռ�����*/
    SysTick->VAL = 0x00;
    /*��ʼ����*/
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk ;
    do
    {
        temp = SysTick->CTRL;
    }
    /*�ȴ�ʱ�䵽��*/
    while ((temp & 0x01) && !(temp & (1 << 16)));
    /*�رռ�����*/
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
    /*��ռ�����*/
    SysTick->VAL = 0X00;
}

/********************************************************************************************************
**������Ϣ ��void UartSendByte(u8 dat)
**�������� ��UART��������
**������� ��dat
**������� ��
**    ��ע ��
********************************************************************************************************/
void UartSendByte(u8 dat)
{
    UART_SendData(UART1, dat);
    while (!UART_GetFlagStatus(UART1, UART_FLAG_TXEPT));
}


/********************************************************************************************************
**������Ϣ ��void UartSendGroup(u8* buf,u16 len)
**�������� ��UART��������
**������� ��buf,len
**������� ��
**    ��ע ��
********************************************************************************************************/
void UartSendGroup(u8 *buf, u16 len)
{
    while (len--)
        UartSendByte(*buf++);
}



void IAP_Updata(void)
{
    if ((UART_RX_BUF[0] == updata_cmd[0]) &&
            (UART_RX_BUF[1] == updata_cmd[1]) &&
            (UART_RX_BUF[2] == updata_cmd[2]) &&
            (UART_RX_BUF[3] == updata_cmd[3]) &&
            (UART_RX_BUF[4] == updata_cmd[4]) &&
            (UART_RX_BUF[5] == updata_cmd[5]))
    {
        printf("mm32_iap_update_start");
        memset(UART_RX_BUF, 0, 100);
        data_length = 0;
        RestartForBoot();
    }
    else
    {
        printf("MM32_IAP_CMD_error");
        memset(UART_RX_BUF, 0, 100);
        data_length = 0;
    }
}



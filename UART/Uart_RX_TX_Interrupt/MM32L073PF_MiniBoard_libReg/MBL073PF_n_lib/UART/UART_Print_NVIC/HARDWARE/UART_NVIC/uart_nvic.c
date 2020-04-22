#include "uart_nvic.h"
void uart_send_dates(u8 *date, u8 len);

//����1�жϷ������
u8 UART_RX_BUF[UART_REC_LEN];     //���ջ���,���UART_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 UART_RX_STA=0;       //����״̬���	  

void uart_nvic_init(u32 bound){
    //GPIO�˿�����
    GPIO_InitTypeDef GPIO_InitStructure;
    UART_InitTypeDef UART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_UART1, ENABLE);	//ʹ��UART1
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);  //����GPIOAʱ��
    
    //UART1 NVIC ����
    NVIC_InitStructure.NVIC_IRQChannel = UART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPriority = 3;		//�����ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
    NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
    
    //UART ��ʼ������
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_1);
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_1);
    
    UART_InitStructure.UART_BaudRate = bound;//���ڲ�����
    UART_InitStructure.UART_WordLength = UART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
    UART_InitStructure.UART_StopBits = UART_StopBits_1;//һ��ֹͣλ
    UART_InitStructure.UART_Parity = UART_Parity_No;//����żУ��λ
    UART_InitStructure.UART_HardwareFlowControl = UART_HardwareFlowControl_None;//��Ӳ������������
    UART_InitStructure.UART_Mode = UART_Mode_Rx | UART_Mode_Tx;	//�շ�ģʽ
    
    UART_Init(UART1, &UART_InitStructure); //��ʼ������1
    UART_ITConfig(UART1, UART_IT_RXIEN, ENABLE);//�������ڽ����ж�
    UART_Cmd(UART1, ENABLE);                    //ʹ�ܴ���1 
    
    //UART1_TX   GPIOA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
    GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.9
    
    //UART1_RX	  GPIOA.10��ʼ��
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
    GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.10  
    
}
#define TxBufferSize1   (countof(TxBuffer1) - 1)
#define RxBufferSize1   (countof(TxBuffer1) - 1)
#define countof(a)   (sizeof(a) / sizeof(*(a)))      //��ʾ����a��Ԫ�صĸ���

uint8_t TxBuffer1[] = "USART Interrupt Example: This is USART1 DEMO";
uint8_t RxBuffer1[RxBufferSize1],rec_f;
__IO uint8_t TxCounter1 = 0x00;
__IO uint8_t RxCounter1 = 0x00;
uint8_t NbrOfDataToTransfer1 = TxBufferSize1;
uint8_t NbrOfDataToRead1 = RxBufferSize1;
void UART1_IRQHandler(void)                	//����1�жϷ������
	{
    u8 Res;
    if(UART_GetITStatus(UART1, UART_IT_RXIEN)  != RESET)  //?????��?(????????????????0x0d 0x0a??��)
    {
        UART_ClearITPendingBit(UART1,UART_IT_RXIEN);
        Res =UART_ReceiveData(UART1);	//??????????????
        if((Res==0x0d)&&((UART_RX_STA&0X3FFF)>0))
        {
            UART_RX_STA|=0x4000;
            UART_RX_BUF[UART_RX_STA&0X3FFF]=Res ;
            UART_RX_STA++;
        }
        else if((UART_RX_STA&0x4000)&&((UART_RX_STA&0X3FFF)>0))//???????0x0d
        {
            if(Res==0x0a)
            {
                UART_RX_STA|=0x8000;
            }
            UART_RX_BUF[UART_RX_STA&0X3FFF]=Res ;
            UART_RX_STA++;
        }
        else{
            UART_RX_BUF[UART_RX_STA&0X3FFF]=Res ;
            UART_RX_STA++;
            UART_RX_STA=UART_RX_STA&0X3FFF;
            if((UART_RX_STA)>(UART_REC_LEN-1))
                UART_RX_STA=0;//???????????,??????????
        }
	//TxBuffer1[UART_RX_STA]  = UART_RX_BUF[UART_RX_STA];
     }
if(UART_GetITStatus(UART1, UART_IT_TXIEN)  != RESET)
  {
	uart_send_dates(UART_RX_BUF,UART_RX_STA);
	UART_ClearITPendingBit(UART1,UART_IT_TXIEN);
	UART_ITConfig(UART1, UART_IT_TXIEN, DISABLE); //�رշ����ж�
  }
}

void uart_send_dates(u8 *date, u8 len){
			u16 t=1;
			for(t=1;t<len;t++)
			{
				UART_SendData(UART1, UART_RX_BUF[t]);//�򴮿�1��������
				while(UART_GetFlagStatus(UART1,UART_IT_TXIEN)!=SET);//�ȴ����ͽ���
			}
}



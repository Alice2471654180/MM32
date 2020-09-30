/**
******************************************************************************
* @file    main.c
* @author  AE Team
* @version V1.1.0
* @date    28/08/2019
* @brief   This file provides all the main firmware functions.
******************************************************************************
* @copy
*
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
* TIME. AS A RESULT, MindMotion SHALL NOT BE HELD LIABLE FOR ANY
* DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
* FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
* CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*
* <h2><center>&copy; COPYRIGHT 2019 MindMotion</center></h2>
*/
#include "led.h"
#include "delay.h"
#include "sys.h"
#include "tim2.h"
#include "uart_nvic.h"
/********************************************************************************************************
**������Ϣ ��int main (void)
**�������� ��1��HSE 96M ; 3.3V ; PA9(UART1-TXD)��PA10(UART1-RXD) ;������=115200,8Bit,1Stop
             2��������ʹ��TIM2��ʱ�����UARTʵ����UART�����жϹ��ܣ����Խ��ղ������ȵ�����֡Ҳ����Ҫ�ض��Ľ�������
						    UART����ʱ����԰�ʵ�������趨��
**������� ��
**������� ��
********************************************************************************************************/
int main(void)
{
    u8 t;
    u8 len;
    u16 times = 0;
	
    delay_init();	    	     //��ʱ������ʼ��
    uart_nvic_init(115200);	 //���ڳ�ʼ��Ϊ115200
	  /*PCLK1��Ƶ��(96MHz)96��Ƶ,������ֵΪ1000,��1ms�ж�һ��*/
    Tim2_UPCount_test(SystemCoreClock / 1000000 - 1, 1000 - 1);//��1ms��ʱ����ΪUART�Ŀ���ʱ�䣬��ʱ����԰�ʵ�������趨
    LED_Init();		  	       //��ʼ����LED���ӵ�Ӳ���ӿ�
	
	  printf("\r\n ʹ��TIM2ģ��UART1�����ж�ʵ��\r\n");
	
    while(1)
    {
        if(UART_RX_STA & 0x8000)//��ʾ����������֡�������
        {
            len = UART_RX_STA & 0x3fff; //�õ��˴ν��յ������ݳ���
            printf("\r\n�����͵���ϢΪ:\r\n");
            for(t = 0; t < len; t++)
            {
                while (UART_GetFlagStatus(UART1, UART_IT_TXIEN) == RESET);
                UART_SendData(UART1, (u8)UART_RX_BUF[t]);
            }
            printf("\r\n\r\n");//���뻻��
            UART_RX_STA = 0;
        }
        else
        {
            times++;
            if(times % 200 == 0)printf("����������,����س�������\r\n");
            if(times % 30 == 0)LED1 = !LED1; //��˸LED,��ʾϵͳ��������.
            delay_ms(10);
        }
    }
}

/**
* @}
*/

/**
* @}
*/

/**
* @}
*/

/*-------------------------(C) COPYRIGHT 2019 MindMotion ----------------------*/


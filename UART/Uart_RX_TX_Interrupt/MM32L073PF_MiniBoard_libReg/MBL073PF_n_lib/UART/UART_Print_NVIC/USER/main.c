#include "delay.h"
#include "sys.h"
#include "uart_nvic.h"
#include "led.h"

int main(void)
{
    u8 t;
    u8 len;	
    u16 times=0; 
    
    delay_init();
    LED_Init();
    uart_nvic_init(115200);	 //���ڳ�ʼ��Ϊ115200
    
    while(1)
    {
			if(UART_RX_STA&0x8000)
			{
				UART_SendData(UART1, UART_RX_BUF[0]);//�򴮿�1��������
				UART_ITConfig(UART1, UART_IT_TXIEN, ENABLE);
				UART_RX_STA=0;
			}
    }	 
}





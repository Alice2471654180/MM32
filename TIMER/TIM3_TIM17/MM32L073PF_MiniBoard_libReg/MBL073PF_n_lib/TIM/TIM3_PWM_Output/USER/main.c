#include "delay.h"
#include "sys.h"
#include "uart.h"
#include "pwm.h"
#include "led.h"
/********************************************************************************************************
**������Ϣ ��int main (void)                          
**�������� ���ϵ�󣬲鿴LED�𽥱�����Ȼ���ֱ䰵����˷���
**������� ��
**������� ��
********************************************************************************************************/

int main(void)
{
    u8 dir=1;	
    u16 led0pwmval=0;         
    delay_init();	
    uart_initwBaudRate(115200);
    LED_Init();
    TIM3_PWM_Init(999,0);//����Ƶ
		TIM17_PWM_Init(999,0);
	  TIM_SetCompare1(TIM3,500);	 
		TIM_SetCompare1(TIM17,500);	
    while(1)                                                                                               //����ѭ��
    {

    }	
}



#include "led_mesh.h"

void PWM_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_OCInitTypeDef  TIM_OCInitStructure;

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3; //TIM3_CH1
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //�����������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5; //TIM3_CH1
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //�����������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

  TIM_TimeBaseStructure.TIM_Period = 255 * 100; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ   1K
  TIM_TimeBaseStructure.TIM_Prescaler = 2; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ  480��Ƶ 100k
  TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ

  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
  TIM_OCInitStructure.TIM_Pulse = 0; //���ô�װ�벶��ȽϼĴ���������ֵ
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //�������:TIM����Ƚϼ��Ը�
  TIM_OC2Init(TIM2, &TIM_OCInitStructure);  //����TIM_OCInitStruct��ָ���Ĳ�����ʼ������TIMx
  TIM_OC3Init(TIM2, &TIM_OCInitStructure);
  TIM_OC4Init(TIM2, &TIM_OCInitStructure);

  TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);  //CH1Ԥװ��ʹ��
  TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);  //CH1Ԥװ��ʹ��
  TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable);  //CH1Ԥװ��ʹ��

  TIM_ARRPreloadConfig(TIM2, ENABLE); //ʹ��TIMx��ARR�ϵ�Ԥװ�ؼĴ���

  TIM_Cmd(TIM2, ENABLE);  //ʹ��TIM1
  GPIOA->AFRL &= 0xFFF000F;
  GPIOA->AFRL |= 0x0002220;

  TIM_TimeBaseStructure.TIM_Period = 255 * 100; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ   1K
  TIM_TimeBaseStructure.TIM_Prescaler = 2; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ  480��Ƶ 100k
  TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ

  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
  TIM_OCInitStructure.TIM_Pulse = 0; //���ô�װ�벶��ȽϼĴ���������ֵ
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //�������:TIM����Ƚϼ��Ը�
  TIM_OC1Init(TIM3, &TIM_OCInitStructure);  //����TIM_OCInitStruct��ָ���Ĳ�����ʼ������TIMx
  TIM_OC2Init(TIM3, &TIM_OCInitStructure);

  TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);  //CH1Ԥװ��ʹ��
  TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);  //CH1Ԥװ��ʹ��

  TIM_ARRPreloadConfig(TIM3, ENABLE); //ʹ��TIMx��ARR�ϵ�Ԥװ�ؼĴ���
  TIM_Cmd(TIM3, ENABLE);  //ʹ��TIM1
  GPIOB->AFRL &= 0xF00FFFF;
  GPIOB->AFRL |= 0x0110000;
}

void UpdateLEDValueAll(void) //porting function
{
  int t;
  unsigned char data[7];
  unsigned char EnableLED_Flag = 0;
  unsigned int Led_Lum_percent = 100;

  Led_getInfo(data);
  EnableLED_Flag = data[0];
  if (EnableLED_Flag == 0)
  {
    TIM_SetCompare2(TIM2, 0); //G
    TIM_SetCompare3(TIM2, 0); //B
    TIM_SetCompare4(TIM2, 0); //R
    TIM_SetCompare1(TIM3, 0); //Y
    TIM_SetCompare2(TIM3, 0); //W
  }
  else
  {
    Led_Lum_percent = data[6];
    t = data[1] * Led_Lum_percent;
    TIM_SetCompare2(TIM3, t); //Rx100
    t = data[2] * Led_Lum_percent;
    TIM_SetCompare1(TIM3, t); //Gx100
    t = data[3] * Led_Lum_percent;
    TIM_SetCompare2(TIM2, t); //Bx100
    t = data[4] * Led_Lum_percent;
    TIM_SetCompare3(TIM2, t); //Yx100
    t = data[5] * Led_Lum_percent;
    TIM_SetCompare4(TIM2, t); //Wx100
  }

}
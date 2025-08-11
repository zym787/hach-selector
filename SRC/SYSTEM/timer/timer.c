#define _TIMER_H_GLOBALS_
#include "common.h"



//ͨ�ö�ʱ��2�жϳ�ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//����ʹ�õ��Ƕ�ʱ��3!
void TIM2_Init(uint16 arr,uint16 psc)
{
	RCC->APB1ENR|=1<<0;	//TIM2ʱ��ʹ��
 	TIM2->ARR=arr;  	//�趨�������Զ���װֵ//�պ�1ms
	TIM2->PSC=psc;  	//Ԥ��Ƶ��7200,�õ�10Khz�ļ���ʱ��
	TIM2->DIER|=1<<0;   //��������ж�
	TIM2->CR1|=0x01;    //ʹ�ܶ�ʱ��2
	MY_NVIC_Init(1,3,TIM2_IRQn,2);//��ռ1�������ȼ�3����2
}

//��ʱ��2�жϷ������
void TIM2_IRQHandler(void)
{
    if(TIM2->SR&0x0001)//����ж�
	{
        TIM2->SR &= ~0x0001 ;//����жϱ�־λ
        ++timerPara.timeDbg;
        ++timerPara.timeWaitMill;
        ++timerPara.everySec;
        if(Valve.status&VALVE_INITING||Valve.status==VALVE_RUNNING)
            ++syspara.protectTimeOut;
        if(protext.stepCnt)
            ++protext.time;
        else
            protext.time = 0;;
        StrProc();
    }
}


//---------------------------------------------------------------------
//ͨ�ö�ʱ��3�жϳ�ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//����ʹ�õ��Ƕ�ʱ��3!
void TIM3_Init(uint16 arr,uint16 psc)
{
	RCC->APB1ENR|=1<<1;	//TIM3ʱ��ʹ��
 	TIM3->ARR=arr;  	//�趨�������Զ���װֵ//�պ�1ms
	TIM3->PSC=psc;  	//Ԥ��Ƶ��7200,�õ�10Khz�ļ���ʱ��
	TIM3->DIER|=1<<0;   //��������ж�
	TIM3->CR1|=0x01;    //ʹ�ܶ�ʱ��3
	MY_NVIC_Init(0,2,TIM3_IRQn,2);//��ռ1�������ȼ�3����2
}

//��ʱ��3�жϷ������
void TIM3_IRQHandler(void)
{
	if(TIM3->SR&0X0001)//����ж�
	{
        TIM3->SR &= ~0x0001 ;//����жϱ�־λ
        ModbusTimesProcess();
	}
}

//---------------------------------------------------------------------
//ͨ�ö�ʱ��4�жϳ�ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//����ʹ�õ��Ƕ�ʱ��4!
void TIM4_Init(uint16 arr,uint16 psc)
{
	RCC->APB1ENR|=1<<2;	//TIM4ʱ��ʹ��
 	TIM4->ARR=arr;  	//�趨�������Զ���װֵ//�պ�1ms
	TIM4->PSC=psc;  	//Ԥ��Ƶ��7200,�õ�10Khz�ļ���ʱ��
	TIM4->DIER|=1<<0;   //��������ж�
	TIM4->CR1|=0x01;    //ʹ�ܶ�ʱ��4
	MY_NVIC_Init(0,3,TIM4_IRQn,2);//��ռ1�������ȼ�3����2
}

//��ʱ��4�жϷ������
void TIM4_IRQHandler(void)
{
	if(TIM4->SR&0X0001)//����ж�
	{
        TIM4->SR &= ~0x0001 ;//����жϱ�־λ
        AxisXTimer();
//        ProcessValveTimer();
	}
}

void HardFault_Handler(void)
{
	static unsigned char flg=0;
    /* Go to infinite loop when Hard Fault exception occurs */
	if(!flg)
	{
		flg = 1;
//        printf("\r\n An HardFault occur");
	}
}
#if 0
//---------------------------------------------------------------------
//ͨ�ö�ʱ��5�жϳ�ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//����ʹ�õ��Ƕ�ʱ��5!
void TIM5_Init(uint16 arr,uint16 psc)
{
	RCC->APB1ENR|=1<<3;	//TIM5ʱ��ʹ��
 	TIM5->ARR=arr;  	//�趨�������Զ���װֵ//�պ�1ms
	TIM5->PSC=psc;  	//Ԥ��Ƶ��7200,�õ�10Khz�ļ���ʱ��
	TIM5->DIER|=1<<0;   //��������ж�
	TIM5->CR1|=0x01;    //ʹ�ܶ�ʱ��3
	MY_NVIC_Init(1,3,TIM5_IRQn,2);//��ռ1�������ȼ�3����2
}

//��ʱ��3�жϷ������
void TIM5_IRQHandler(void)
{
	if(TIM5->SR&0X0001)//����ж�
	{
        TIM5->SR &= ~0x0001 ;//����жϱ�־λ
	}
}

//---------------------------------------------------------------------
//ͨ�ö�ʱ��6�жϳ�ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//����ʹ�õ��Ƕ�ʱ��6!
void TIM6_Init(uint16 arr,uint16 psc)
{
	RCC->APB1ENR|=1<<4;	//TIM6ʱ��ʹ��
 	TIM6->ARR=arr;  	//�趨�������Զ���װֵ//�պ�1ms
	TIM6->PSC=psc;  	//Ԥ��Ƶ��7200,�õ�10Khz�ļ���ʱ��
	TIM6->DIER|=1<<0;   //��������ж�
	TIM6->CR1|=0x01;    //ʹ�ܶ�ʱ��6
	MY_NVIC_Init(1,3,TIM6_IRQn,2);//��ռ1�������ȼ�3����2
}

//��ʱ��3�жϷ������
void TIM6_IRQHandler(void)
{
	if(TIM6->SR&0X0001)//����ж�
	{
        TIM6->SR &= ~0x0001 ;//����жϱ�־λ
	}
}


//---------------------------------------------------------------------
//ͨ�ö�ʱ��7�жϳ�ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//����ʹ�õ��Ƕ�ʱ��7!
void TIM7_Init(uint16 arr,uint16 psc)
{
	RCC->APB1ENR|=1<<5;	//TIM3ʱ��ʹ��
 	TIM7->ARR=arr;  	//�趨�������Զ���װֵ//�պ�1ms
	TIM7->PSC=psc;  	//Ԥ��Ƶ��7200,�õ�10Khz�ļ���ʱ��
	TIM7->DIER|=1<<0;   //��������ж�
	TIM7->CR1|=0x01;    //ʹ�ܶ�ʱ��3
	MY_NVIC_Init(1,3,TIM7_IRQn,2);//��ռ1�������ȼ�3����2
}

//��ʱ��7�жϷ������
void TIM7_IRQHandler(void)
{
	if(TIM7->SR&0X0001)//����ж�
	{
        TIM7->SR &= ~0x0001 ;//����жϱ�־λ
	}
}

#endif



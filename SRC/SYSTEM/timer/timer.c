#define _TIMER_H_GLOBALS_
#include "common.h"



//通用定时器2中断初始化
//这里时钟选择为APB1的2倍，而APB1为36M
//arr：自动重装值。
//psc：时钟预分频数
//这里使用的是定时器3!
void TIM2_Init(uint16 arr,uint16 psc)
{
	RCC->APB1ENR|=1<<0;	//TIM2时钟使能
 	TIM2->ARR=arr;  	//设定计数器自动重装值//刚好1ms
	TIM2->PSC=psc;  	//预分频器7200,得到10Khz的计数时钟
	TIM2->DIER|=1<<0;   //允许更新中断
	TIM2->CR1|=0x01;    //使能定时器2
	MY_NVIC_Init(1,3,TIM2_IRQn,2);//抢占1，子优先级3，组2
}

//定时器2中断服务程序
void TIM2_IRQHandler(void)
{
    if(TIM2->SR&0x0001)//溢出中断
	{
        TIM2->SR &= ~0x0001 ;//清除中断标志位
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
//通用定时器3中断初始化
//这里时钟选择为APB1的2倍，而APB1为36M
//arr：自动重装值。
//psc：时钟预分频数
//这里使用的是定时器3!
void TIM3_Init(uint16 arr,uint16 psc)
{
	RCC->APB1ENR|=1<<1;	//TIM3时钟使能
 	TIM3->ARR=arr;  	//设定计数器自动重装值//刚好1ms
	TIM3->PSC=psc;  	//预分频器7200,得到10Khz的计数时钟
	TIM3->DIER|=1<<0;   //允许更新中断
	TIM3->CR1|=0x01;    //使能定时器3
	MY_NVIC_Init(0,2,TIM3_IRQn,2);//抢占1，子优先级3，组2
}

//定时器3中断服务程序
void TIM3_IRQHandler(void)
{
	if(TIM3->SR&0X0001)//溢出中断
	{
        TIM3->SR &= ~0x0001 ;//清除中断标志位
        ModbusTimesProcess();
	}
}

//---------------------------------------------------------------------
//通用定时器4中断初始化
//这里时钟选择为APB1的2倍，而APB1为36M
//arr：自动重装值。
//psc：时钟预分频数
//这里使用的是定时器4!
void TIM4_Init(uint16 arr,uint16 psc)
{
	RCC->APB1ENR|=1<<2;	//TIM4时钟使能
 	TIM4->ARR=arr;  	//设定计数器自动重装值//刚好1ms
	TIM4->PSC=psc;  	//预分频器7200,得到10Khz的计数时钟
	TIM4->DIER|=1<<0;   //允许更新中断
	TIM4->CR1|=0x01;    //使能定时器4
	MY_NVIC_Init(0,3,TIM4_IRQn,2);//抢占1，子优先级3，组2
}

//定时器4中断服务程序
void TIM4_IRQHandler(void)
{
	if(TIM4->SR&0X0001)//溢出中断
	{
        TIM4->SR &= ~0x0001 ;//清除中断标志位
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
//通用定时器5中断初始化
//这里时钟选择为APB1的2倍，而APB1为36M
//arr：自动重装值。
//psc：时钟预分频数
//这里使用的是定时器5!
void TIM5_Init(uint16 arr,uint16 psc)
{
	RCC->APB1ENR|=1<<3;	//TIM5时钟使能
 	TIM5->ARR=arr;  	//设定计数器自动重装值//刚好1ms
	TIM5->PSC=psc;  	//预分频器7200,得到10Khz的计数时钟
	TIM5->DIER|=1<<0;   //允许更新中断
	TIM5->CR1|=0x01;    //使能定时器3
	MY_NVIC_Init(1,3,TIM5_IRQn,2);//抢占1，子优先级3，组2
}

//定时器3中断服务程序
void TIM5_IRQHandler(void)
{
	if(TIM5->SR&0X0001)//溢出中断
	{
        TIM5->SR &= ~0x0001 ;//清除中断标志位
	}
}

//---------------------------------------------------------------------
//通用定时器6中断初始化
//这里时钟选择为APB1的2倍，而APB1为36M
//arr：自动重装值。
//psc：时钟预分频数
//这里使用的是定时器6!
void TIM6_Init(uint16 arr,uint16 psc)
{
	RCC->APB1ENR|=1<<4;	//TIM6时钟使能
 	TIM6->ARR=arr;  	//设定计数器自动重装值//刚好1ms
	TIM6->PSC=psc;  	//预分频器7200,得到10Khz的计数时钟
	TIM6->DIER|=1<<0;   //允许更新中断
	TIM6->CR1|=0x01;    //使能定时器6
	MY_NVIC_Init(1,3,TIM6_IRQn,2);//抢占1，子优先级3，组2
}

//定时器3中断服务程序
void TIM6_IRQHandler(void)
{
	if(TIM6->SR&0X0001)//溢出中断
	{
        TIM6->SR &= ~0x0001 ;//清除中断标志位
	}
}


//---------------------------------------------------------------------
//通用定时器7中断初始化
//这里时钟选择为APB1的2倍，而APB1为36M
//arr：自动重装值。
//psc：时钟预分频数
//这里使用的是定时器7!
void TIM7_Init(uint16 arr,uint16 psc)
{
	RCC->APB1ENR|=1<<5;	//TIM3时钟使能
 	TIM7->ARR=arr;  	//设定计数器自动重装值//刚好1ms
	TIM7->PSC=psc;  	//预分频器7200,得到10Khz的计数时钟
	TIM7->DIER|=1<<0;   //允许更新中断
	TIM7->CR1|=0x01;    //使能定时器3
	MY_NVIC_Init(1,3,TIM7_IRQn,2);//抢占1，子优先级3，组2
}

//定时器7中断服务程序
void TIM7_IRQHandler(void)
{
	if(TIM7->SR&0X0001)//溢出中断
	{
        TIM7->SR &= ~0x0001 ;//清除中断标志位
	}
}

#endif



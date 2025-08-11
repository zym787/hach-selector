#define _DMA_GLOBALS_
#include "common.h"


/*
    Just Do It
    U don't know why i don't know why only ST knows why
*/
void Napping(void)
{
    delay_ms(1);
}

/*

*/
void ReveiveTransmitShift(void)
{
    delay_ms(1);
}

/*
    串口用DMA方式发送
    DMA功能配置
    1、使能时钟
    2、外设地址
    3、设置存储器地址
    4、设置传输数据量
    5、使能通道，启动传输
*/
void MYDMA_TX_Cfg(DMA_Channel_TypeDef*DMA_CHx, uint32 cpar, uint32 cmar, uint16 cndtr)
{
    RCC->AHBENR|=RCC_AHBPeriph_DMA1;        //开启DMA1时钟
    delay_ms(5);                            //等待DMA时钟稳定，必须延时
    DMA_CHx->CPAR=cpar;                     //DMA1 外设地址
    DMA_CHx->CMAR=(uint32)cmar;             //DMA1,存储器地址
    DMA_CHx->CNDTR=cndtr;                   //DMA1,传输数据量

    DMA_CHx->CCR=0;                         //复位
    DMA_CHx->CCR |= (1<<1);                 //使能完成中断
    DMA_CHx->CCR |= 1<<4;                   //从存储器读
    DMA_CHx->CCR &= ~(1<<5);                //普通模式
    DMA_CHx->CCR &= ~(1<<6);                //外设地址非增量模式
    DMA_CHx->CCR |= 1<<7;                   //存储器增量模式
    DMA_CHx->CCR &= ~(1<<8);                //外设数据宽度为8位
    DMA_CHx->CCR &= ~(1<<10);               //存储器数据宽度8位
    DMA_CHx->CCR |= 1<<12;                  //中等优先级
    DMA_CHx->CCR &= ~(1<<14);               //非存储器到存储器模式
    MY_NVIC_Init(3,3,DMA1_Channel2_IRQn,2);
    MY_NVIC_Init(3,3,DMA1_Channel7_IRQn,2);
}


/*

*/
void MYDMA_Transmit_Enable(USART_TypeDef *UART_CHx, DMA_Channel_TypeDef *DMA_CHx, uint16 buffLen)
{
    UART_CHx->CR3|=1<<7;                        //使能串口的DMA发送
    DMA_CHx->CCR&=~(1<<0);                      //关闭DMA传输
    DMA_CHx->CNDTR=(uint32)buffLen;             //DMA1,传输数据量
    DMA_CHx->CCR|=(1<<0);                       //开启DMA传输
}

/*
    DMA发送完成中断
*/
void DMA1_Channel2_IRQHandler(void)
{
    if(DMA1->ISR&(1<<5))               //等待通道4传输完成 BIT: TCIF
    {
        DMA1->IFCR|=1<<5;              //清除通道4传输完成标志 BIT: CTCIF
        Napping();
        RX_EN();
        MYDMA_Receive_Enable(USART3, DMA1_Channel3, RECEIVE_LENS);
    }
}

/*
    DMA发送完成中断
*/
void DMA1_Channel7_IRQHandler(void)
{
    if(DMA1->ISR&(1<<25))               //等待通道4传输完成 BIT: TCIF
    {
        DMA1->IFCR|=1<<25;              //清除通道4传输完成标志 BIT: CTCIF
        Napping();
        MYDMA_Receive_Enable(USART2, DMA1_Channel6, RECEIVE_LENS);
    }
}


/*

*/
void MYDMA_RX_Cfg(DMA_Channel_TypeDef*DMA_CHx, unsigned int cpar, unsigned int cmar, unsigned short cndtr)
{
    RCC->AHBENR|=RCC_AHBPeriph_DMA1;    //开启DMA1时钟
    delay_ms(5);                        //等待DMA时钟稳定，必须延时
    DMA_CHx->CPAR=cpar;                 //DMA1 外设地址 
    DMA_CHx->CMAR=(unsigned int)cmar;   //DMA1,存储器地址
    DMA_CHx->CNDTR=cndtr;               //DMA1,传输数据量
    
    DMA_CHx->CCR=0;                     //复位
    DMA_CHx->CCR |= (1<<1);             //使能完成中断
    DMA_CHx->CCR&=~(1<<4);              //从外设串口读
    DMA_CHx->CCR&=~(1<<5);              //普通模式
    DMA_CHx->CCR&=~(1<<6);              //外设地址非增量模式
    DMA_CHx->CCR|=1<<7;                 //存储器增量模式
    DMA_CHx->CCR&=~(1<<8);              //外设数据宽度为8位
    DMA_CHx->CCR&=~(1<<10);             //存储器数据宽度8位
    DMA_CHx->CCR|=1<<12;                //中等优先级
    DMA_CHx->CCR&=~(1<<14);             //非存储器到存储器模式

    DMA_CHx->CCR|=(1<<0);               //启动DMA
    MY_NVIC_Init(3,3,DMA1_Channel3_IRQn,2);
    MY_NVIC_Init(3,3,DMA1_Channel6_IRQn,2);
} 


/*

*/
void MYDMA_Receive_Enable(USART_TypeDef *UART_CHx, DMA_Channel_TypeDef *DMA_CHx, uint16 buffLen)
{
    UART_CHx->CR3|=1<<6;                        //使能串口的DMA接收
    DMA_CHx->CCR&=~(1<<0);                      //关闭DMA传输
    DMA_CHx->CNDTR=(uint32)buffLen;             //DMA1,传输数据量
    DMA_CHx->CCR|=(1<<0);                       //开启DMA传输
}

/*
    DMA接收完成中断
*/
void DMA1_Channel3_IRQHandler(void)
{
    if(DMA1->ISR&(1<<9))               //等待通道3传输完成 BIT: TCIF
    {
        DMA1->IFCR|=1<<9;              //清除通道3传输完成标志 BIT: CTCIF      
        Napping();
		if(*(protext.usartBuf+0) == HEAD_BYTE && *(protext.usartBuf+1) == ModbusPara.mAddrs)
		{
            protext.rxCount = RECEIVE_LENS;
            protext.stepCnt = PROTOCOL_OK;
		}
        else
        {
            ERR_Reset();
        }
        MYDMA_Receive_Enable(USART3, DMA1_Channel3, RECEIVE_LENS);
    }
}

/*
    DMA接收完成中断
*/
void DMA1_Channel6_IRQHandler(void)
{
    if(DMA1->ISR&(1<<21))               //等待通道6传输完成 BIT: TCIF
    {
        DMA1->IFCR|=1<<21;              //清除通道6传输完成标志 BIT: CTCIF
        Napping();
		if(*(protext.usartBuf+0) == HEAD_BYTE && *(protext.usartBuf+1) == ModbusPara.mAddrs)
		{
            protext.rxCount = RECEIVE_LENS;
            protext.stepCnt = PROTOCOL_OK;
		}
        else
        {
            ERR_Reset();        
        }
        MYDMA_Receive_Enable(USART2, DMA1_Channel6, RECEIVE_LENS);
    }
}



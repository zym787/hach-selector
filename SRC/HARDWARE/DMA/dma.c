#define _DMA_GLOBALS_
#include "common.h"


//-------------------------DMA 串口2，485MODBUS通信-------------------------------------------------------------//
//使用方法：先配置发送/接收，后续使能DMA进行一次全新的DMA传输。
//MYDMA_TX_Cfg 发送配置
//MYDMA_RX_Cfg 接收配置
//MYDMA_Enable 使能传输

//接收完成后需要读取SR与DR来清除DMA接收标志
//temp = USART2->SR;
//temp = USART2->DR;



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
void MYDMA_Enable(USART_TypeDef *UART_CHx, DMA_Channel_TypeDef *DMA_CHx, uint16 buffLen)
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
        RX_EN();
//        printd("\r\n DMA12 Transmit Over!!");
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
//        printd("\r\n DMA17 Transmit Over!!");
    }
}



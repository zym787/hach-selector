#define _DMA_GLOBALS_
#include "common.h"


//-------------------------DMA ����2��485MODBUSͨ��-------------------------------------------------------------//
//ʹ�÷����������÷���/���գ�����ʹ��DMA����һ��ȫ�µ�DMA���䡣
//MYDMA_TX_Cfg ��������
//MYDMA_RX_Cfg ��������
//MYDMA_Enable ʹ�ܴ���

//������ɺ���Ҫ��ȡSR��DR�����DMA���ձ�־
//temp = USART2->SR;
//temp = USART2->DR;



/*
    ������DMA��ʽ����
    DMA��������
    1��ʹ��ʱ��
    2�������ַ
    3�����ô洢����ַ
    4�����ô���������
    5��ʹ��ͨ������������
*/
void MYDMA_TX_Cfg(DMA_Channel_TypeDef*DMA_CHx, uint32 cpar, uint32 cmar, uint16 cndtr)
{
    RCC->AHBENR|=RCC_AHBPeriph_DMA1;        //����DMA1ʱ��
    delay_ms(5);                            //�ȴ�DMAʱ���ȶ���������ʱ
    DMA_CHx->CPAR=cpar;                     //DMA1 �����ַ
    DMA_CHx->CMAR=(uint32)cmar;             //DMA1,�洢����ַ
    DMA_CHx->CNDTR=cndtr;                   //DMA1,����������

    DMA_CHx->CCR=0;                         //��λ
    DMA_CHx->CCR |= (1<<1);                 //ʹ������ж�
    DMA_CHx->CCR |= 1<<4;                   //�Ӵ洢����
    DMA_CHx->CCR &= ~(1<<5);                //��ͨģʽ
    DMA_CHx->CCR &= ~(1<<6);                //�����ַ������ģʽ
    DMA_CHx->CCR |= 1<<7;                   //�洢������ģʽ
    DMA_CHx->CCR &= ~(1<<8);                //�������ݿ��Ϊ8λ
    DMA_CHx->CCR &= ~(1<<10);               //�洢�����ݿ��8λ
    DMA_CHx->CCR |= 1<<12;                  //�е����ȼ�
    DMA_CHx->CCR &= ~(1<<14);               //�Ǵ洢�����洢��ģʽ
    MY_NVIC_Init(3,3,DMA1_Channel2_IRQn,2);
    MY_NVIC_Init(3,3,DMA1_Channel7_IRQn,2);
}

/*

*/
void MYDMA_Enable(USART_TypeDef *UART_CHx, DMA_Channel_TypeDef *DMA_CHx, uint16 buffLen)
{
    UART_CHx->CR3|=1<<7;                        //ʹ�ܴ��ڵ�DMA����
    DMA_CHx->CCR&=~(1<<0);                      //�ر�DMA����
    DMA_CHx->CNDTR=(uint32)buffLen;             //DMA1,����������
    DMA_CHx->CCR|=(1<<0);                       //����DMA����
}

/*
    DMA��������ж�
*/
void DMA1_Channel2_IRQHandler(void)
{
    if(DMA1->ISR&(1<<5))               //�ȴ�ͨ��4������� BIT: TCIF
    {
        DMA1->IFCR|=1<<5;              //���ͨ��4������ɱ�־ BIT: CTCIF
        RX_EN();
//        printd("\r\n DMA12 Transmit Over!!");
    }
}

/*
    DMA��������ж�
*/
void DMA1_Channel7_IRQHandler(void)
{
    if(DMA1->ISR&(1<<25))               //�ȴ�ͨ��4������� BIT: TCIF
    {
        DMA1->IFCR|=1<<25;              //���ͨ��4������ɱ�־ BIT: CTCIF
//        printd("\r\n DMA17 Transmit Over!!");
    }
}



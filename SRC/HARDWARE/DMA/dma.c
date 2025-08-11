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
void MYDMA_Transmit_Enable(USART_TypeDef *UART_CHx, DMA_Channel_TypeDef *DMA_CHx, uint16 buffLen)
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
        Napping();
        RX_EN();
        MYDMA_Receive_Enable(USART3, DMA1_Channel3, RECEIVE_LENS);
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
        Napping();
        MYDMA_Receive_Enable(USART2, DMA1_Channel6, RECEIVE_LENS);
    }
}


/*

*/
void MYDMA_RX_Cfg(DMA_Channel_TypeDef*DMA_CHx, unsigned int cpar, unsigned int cmar, unsigned short cndtr)
{
    RCC->AHBENR|=RCC_AHBPeriph_DMA1;    //����DMA1ʱ��
    delay_ms(5);                        //�ȴ�DMAʱ���ȶ���������ʱ
    DMA_CHx->CPAR=cpar;                 //DMA1 �����ַ 
    DMA_CHx->CMAR=(unsigned int)cmar;   //DMA1,�洢����ַ
    DMA_CHx->CNDTR=cndtr;               //DMA1,����������
    
    DMA_CHx->CCR=0;                     //��λ
    DMA_CHx->CCR |= (1<<1);             //ʹ������ж�
    DMA_CHx->CCR&=~(1<<4);              //�����贮�ڶ�
    DMA_CHx->CCR&=~(1<<5);              //��ͨģʽ
    DMA_CHx->CCR&=~(1<<6);              //�����ַ������ģʽ
    DMA_CHx->CCR|=1<<7;                 //�洢������ģʽ
    DMA_CHx->CCR&=~(1<<8);              //�������ݿ��Ϊ8λ
    DMA_CHx->CCR&=~(1<<10);             //�洢�����ݿ��8λ
    DMA_CHx->CCR|=1<<12;                //�е����ȼ�
    DMA_CHx->CCR&=~(1<<14);             //�Ǵ洢�����洢��ģʽ

    DMA_CHx->CCR|=(1<<0);               //����DMA
    MY_NVIC_Init(3,3,DMA1_Channel3_IRQn,2);
    MY_NVIC_Init(3,3,DMA1_Channel6_IRQn,2);
} 


/*

*/
void MYDMA_Receive_Enable(USART_TypeDef *UART_CHx, DMA_Channel_TypeDef *DMA_CHx, uint16 buffLen)
{
    UART_CHx->CR3|=1<<6;                        //ʹ�ܴ��ڵ�DMA����
    DMA_CHx->CCR&=~(1<<0);                      //�ر�DMA����
    DMA_CHx->CNDTR=(uint32)buffLen;             //DMA1,����������
    DMA_CHx->CCR|=(1<<0);                       //����DMA����
}

/*
    DMA��������ж�
*/
void DMA1_Channel3_IRQHandler(void)
{
    if(DMA1->ISR&(1<<9))               //�ȴ�ͨ��3������� BIT: TCIF
    {
        DMA1->IFCR|=1<<9;              //���ͨ��3������ɱ�־ BIT: CTCIF      
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
    DMA��������ж�
*/
void DMA1_Channel6_IRQHandler(void)
{
    if(DMA1->ISR&(1<<21))               //�ȴ�ͨ��6������� BIT: TCIF
    {
        DMA1->IFCR|=1<<21;              //���ͨ��6������ɱ�־ BIT: CTCIF
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



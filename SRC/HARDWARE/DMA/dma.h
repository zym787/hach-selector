#ifndef _DMA_H_
#define _DMA_H_

#ifdef _DMA_GLOBALS_
#define PEXT
#else
#define PEXT extern
#endif


#define RCC_AHBPeriph_DMA1               ((unsigned int)0x00000001)
#define RCC_AHBPeriph_DMA2               ((unsigned int)0x00000002)

#define BUFF_LEN    32
PEXT uint8 Sndbuf[BUFF_LEN];
PEXT uint8 RcvBuf[BUFF_LEN];


PEXT void MYDMA_TX_Cfg(DMA_Channel_TypeDef*DMA_CHx,unsigned int cpar,unsigned int cmar,unsigned short cndtr);
PEXT void MYDMA_RX_Cfg(DMA_Channel_TypeDef*DMA_CHx,unsigned int cpar,unsigned int cmar,unsigned short cndtr);
PEXT void MYDMA_Enable(USART_TypeDef *UART_CHx, DMA_Channel_TypeDef *DMA_CHx, unsigned short buffLen);
PEXT void DMA1_Channel7_IRQHandler(void);
PEXT void DMA_Usart_Int(void);



#undef PEXT
#endif


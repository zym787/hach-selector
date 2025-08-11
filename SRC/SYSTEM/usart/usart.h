#ifndef _USART_H_
#define _USART_H_

#ifdef _USART_H_GLOBALS_
#define PEXT
#else
#define PEXT extern
#endif


#define USART_REC_LEN  			200  	//�����������ֽ��� 200
#define EN_UART1_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����
#define EN_UART2_RX 			1		//ʹ�ܣ�1��/��ֹ��0������2����
#define EN_UART3_RX 			1		//ʹ�ܣ�1��/��ֹ��0������3����
#define EN_UART4_RX 			1		//ʹ�ܣ�1��/��ֹ��0������4����
	  	
PEXT unsigned char  USART_RX_BUF[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
PEXT unsigned short USART_RX_STA;         		//����״̬���	


//����봮���жϽ��գ��벻Ҫע�����º궨��
PEXT void Usart1_Init(u32 pclk2,u32 bound);
PEXT void USART1_IRQHandler(void);
PEXT void Usart2_Init(u32 pclk2,u32 bound);
PEXT void USART2_IRQHandler(void);
PEXT void Usart2_SendB(unsigned char ch);
PEXT void USART2_SendStr(char *s);
PEXT void Usart3_Init(u32 pclk2,u32 bound);
PEXT void USART3_IRQHandler(void);
PEXT void Usart3_SendB(unsigned char ch);
PEXT void USART3_SendStr(char *s);
PEXT void Uart4_Init(u32 pclk2,u32 bound);
PEXT void UART4_IRQHandler(void);
PEXT void Uart4_SendB(unsigned char ch);
PEXT void UART4_SendStr(char *s);



#undef PEXT
#endif
















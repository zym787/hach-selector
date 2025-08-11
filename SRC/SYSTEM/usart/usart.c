#define _USART_H_GLOBALS_
#include "common.h"


//�������´���,֧��printf����,������Ҫѡ��use MicroLIB
#if 1
#pragma import(__use_no_semihosting)
//��׼����Ҫ��֧�ֺ���
struct __FILE
{
	int handle;
	/* Whatever you require here. If the only file you are using is */
	/* standard output using printf() for debugging, no file handling */
	/* is required. */
};
/* FILE is typedef�� d in stdio.h. */
FILE __stdout;
//����_sys_exit()�Ա���ʹ�ð�����ģʽ
void _sys_exit(int x)
{
	x = x;
}
//�ض���fputc����
int fputc(int ch, FILE *f)
{
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������
	USART1->DR = (u8) ch;
	return ch;
}
#endif



//��ʼ��IO ����1
//pclk2:PCLK2ʱ��Ƶ��(Mhz)
//bound:������
//CHECK OK
void Usart1_Init(u32 pclk2,u32 bound)
{
	float temp;
	u16 mantissa;
	u16 fraction;
	temp=(float)(pclk2*1000000)/(bound*16);//�õ�USARTDIV
	mantissa=temp;				            //�õ���������
	fraction=(temp-mantissa)*16;            //�õ�С������
    mantissa<<=4;
	mantissa+=fraction;

	RCC->APB2ENR |= (RCC_APB2Periph_USART1);            //ʹ�ܴ���1ʱ��
	RCC->APB2ENR |= (RCC_APB2Periph_GPIOA);             //ʹ��PORTA��ʱ��
	GPIOA->CRH &= (GPIO_Crh_P9 & GPIO_Crh_P10);        //IO״̬���� : USART1_TX   PA.9,USART1_RX	  PA.10
	GPIOA->CRH |= (GPIO_Mode_AF_PP_50MHz_P9 | GPIO_Mode_IN_PU_PD_P10);	//IO״̬����:TX---�����������,RX---��������

	RCC->APB2RSTR |= RCC_APB2Periph_USART1;                   //��λ����1
	RCC->APB2RSTR &= ~RCC_APB2Periph_USART1;                //ֹͣ��λ

	//����������
 	USART1->BRR=mantissa;                   //����������
	USART1->CR1|=0x200C;                   // 1λֹͣ,��У��λ.
#if EN_UART1_RX		                        //���ʹ���˽���
	//ʹ�ܽ����ж�
	USART1->CR1|=1<<8;                      //PE�ж�ʹ��
	USART1->CR1|=1<<5;                      //���ջ������ǿ��ж�ʹ��
	MY_NVIC_Init(1,3,USART1_IRQn,2);        //��2��������ȼ�
#endif
}

void DebugSend(unsigned char ch)
{
	while((USART1->SR&0x40)==0);//�ȴ����ͽ���
	USART1->DR=ch;
}

void USART1_IRQHandler(void)
{
	volatile u8 res;
	res = res;
	if(USART1->SR&(1<<5))//���յ�����
	{
		res=USART1->DR;
        getSerialData(res);
	}
}


//----------------------------------------------------------------------------------
//��ʼ��IO ����2
//pclk2:PCLK2ʱ��Ƶ��(Mhz)
//bound:������
//CHECK OK
void Usart2_Init(u32 pclk2,u32 bound)
{
	float temp;
	u16 mantissa;
	u16 fraction;
	//   Tx / Rx  ������ ��  fck/(16*( USARTDIV))
	temp=(float)(pclk2*1000000)/(bound*16);//�õ�USARTDIV
	mantissa=temp;				    //�õ��������� ----�޷������� �� ������������
	fraction=(temp-mantissa)*16;    //�õ�С������	 ������ - �޷������� = ������
    mantissa<<=4;                   // USARTDIV���������� ---bit4~bit15
	mantissa+=fraction;             // USARTDIV��С������ ---bit0~bit3

	RCC->APB2ENR |= (RCC_APB2Periph_GPIOA);   //ʹ��PORTA��ʱ��
	RCC->APB1ENR |= (RCC_APB1Periph_USART2);  //ʹ�ܴ���2ʱ��
	GPIOA->CRL &= (GPIO_Crl_P2 & GPIO_Crl_P3);		//IO״̬���� : USART2_TX   PA.2,USART1_RX	  PA.3
	GPIOA->CRL |= (GPIO_Mode_AF_PP_50MHz_P2 | GPIO_Mode_IN_PU_PD_P3);	//IO״̬����:TX---�����������,RX---��������

	RCC->APB1RSTR |= RCC_APB1Periph_USART2;   //��λ����1
	RCC->APB1RSTR &= ~RCC_APB1Periph_USART2;//ֹͣ��λ

	//����������
 	USART2->BRR=mantissa;            // ����������
	USART2->CR1 |= 0x200C;           //Ĭ��:һ����ʼλ��8������λ,1λֹͣ,��У��λ.
    #if EN_UART2_RX 		         //���ʹ���˽���
	//ʹ�ܽ����ж�
	USART2->CR1 |= 1<<8;              //PE�ж�ʹ��,���ջ������ǿ��ж�ʹ��
	USART2->CR1 |= 1<<5;              //���ջ������ǿ��ж�ʹ��
	MY_NVIC_Init(1,3,USART2_IRQn,2); //��2(4����ռ(0,1,2,3)��4������(0,1,2,3))�������ռ����������ȼ�
    #endif
}


void Usart2_SendB(unsigned char ch)
{
	while((USART2->SR&0x40)==0);//�ȴ����ͽ���
	USART2->DR=ch;
}

//����2�����ַ���
void USART2_SendStr(char *s)
{
    while(*s != '\0') //����ַ���������
    {
        Usart2_SendB(*s++);//���͵�ǰ�ַ�
    }
}

void USART2_IRQHandler(void)
{
	char res;
	res = res;
	if(USART2->SR&(1<<5))//���յ�����
	{
		res=USART2->DR;
        if(syspara.typeProtocal==MY_MODBUS)
            ModbusReceive(res);
        else
            RxUsart(res);
    }
}


//----------------------------------------------------------------------------------
//��ʼ��IO ����3
//pclk2:PCLK2ʱ��Ƶ��(Mhz)
//bound:������
//CHECK OK
void Usart3_Init(u32 pclk2,u32 bound)
{
	float temp;
	u16 mantissa;
	u16 fraction;
	//   Tx / Rx  ������ ��  fck/(16*( USARTDIV))
	temp=(float)(pclk2*1000000)/(bound*16);//�õ�USARTDIV
	mantissa=temp;				    //�õ��������� ----�޷������� �� ������������
	fraction=(temp-mantissa)*16;    //�õ�С������	 ������ - �޷������� = ������
    mantissa<<=4;                   // USARTDIV���������� ---bit4~bit15
	mantissa+=fraction;             // USARTDIV��С������ ---bit0~bit3

	RCC->APB2ENR |= (RCC_APB2Periph_GPIOB);     //ʹ��PORTB��ʱ��
	RCC->APB1ENR |= (RCC_APB1Periph_USART3);     //ʹ�ܴ���3ʱ��
	GPIOB->CRH &= (GPIO_Crh_P10 & GPIO_Crh_P11);		//IO״̬���� : USART3_TX   PC.11,USART3_RX	  PC.10
	GPIOB->CRH |= (GPIO_Mode_AF_PP_50MHz_P10 | GPIO_Mode_IN_PU_PD_P11);	//IO״̬����:TX---�����������,RX---��������

	RCC->APB1RSTR |= RCC_APB1Periph_USART3;     //��λ����1
	RCC->APB1RSTR &= ~RCC_APB1Periph_USART3;    //ֹͣ��λ

	//����������
 	USART3->BRR=mantissa;            // ����������
	USART3->CR1 |= 0x200C;           //Ĭ��:һ����ʼλ��8������λ,1λֹͣ,��У��λ.
    #if EN_UART3_RX 		        //���ʹ���˽���
	//ʹ�ܽ����ж�
	USART3->CR1|= 1<<8;              //PE�ж�ʹ��,���ջ������ǿ��ж�ʹ��
	USART3->CR1|= 1<<5;              //���ջ������ǿ��ж�ʹ��
	MY_NVIC_Init(0,3,USART3_IRQn,2); //��2(4����ռ(0,1,2,3)��4������(0,1,2,3))�������ռ����������ȼ�
    #endif
}


void Usart3_SendB(unsigned char ch)
{
	while((USART3->SR&0x40)==0);//�ȴ����ͽ���
	USART3->DR=ch;
}


//����4�����ַ���
void USART3_SendStr(char *s)
{
    while(*s != '\0') //����ַ���������
    {
        Usart3_SendB(*s++);//���͵�ǰ�ַ�
    }
}


void USART3_IRQHandler(void)
{
	volatile u8 res;
	res = res;
	if(USART3->SR&(1<<5))//���յ�����
	{
		res = USART3->DR;
        if(syspara.typeProtocal==MY_MODBUS)
            ModbusReceive(res);
        else
            RxUsart(res);
	}
}

#if 0
//----------------------------------------------------------------------------------
//��ʼ��IO ����4
//pclk2:PCLK2ʱ��Ƶ��(Mhz)
//bound:������
//CHECK OK
void Uart4_Init(u32 pclk2,u32 bound)
{
	float temp;
	u16 mantissa;
	u16 fraction;
	//   Tx / Rx  ������ ��  fck/(16*( USARTDIV))
	temp=(float)(pclk2*1000000)/(bound*16);//�õ�USARTDIV
	mantissa=temp;				    //�õ��������� ----�޷������� �� ������������
	fraction=(temp-mantissa)*16;    //�õ�С������	 ������ - �޷������� = ������
    mantissa<<=4;                   // USARTDIV���������� ---bit4~bit15
	mantissa+=fraction;             // USARTDIV��С������ ---bit0~bit3

	RCC->APB2ENR |= (RCC_APB2Periph_GPIOC);   //ʹ��PORTA��ʱ��
	RCC->APB1ENR |= (RCC_APB1Periph_UART4);   //ʹ�ܴ���4ʱ��
	GPIOC->CRH &= (GPIO_Crh_P10 & GPIO_Crh_P11);		//IO״̬���� : USART4_TX   PC.11,USART4_RX	  PC.10
	GPIOC->CRH |= (GPIO_Mode_IN_PU_PD_P11 | GPIO_Mode_AF_PP_50MHz_P10);	//IO״̬����:TX---�����������,RX---��������

	RCC->APB1RSTR |= RCC_APB1Periph_UART4;   //��λ����1
	RCC->APB1RSTR &= ~RCC_APB1Periph_UART4;//ֹͣ��λ

	//����������
 	UART4->BRR=mantissa;            // ����������
	UART4->CR1 |= 0x200C;           //Ĭ��:һ����ʼλ��8������λ,1λֹͣ,��У��λ.
    #if EN_UART4_RX 		        //���ʹ���˽���
	//ʹ�ܽ����ж�
	UART4->CR1|= 1<<8;              //PE�ж�ʹ��,���ջ������ǿ��ж�ʹ��
	UART4->CR1|= 1<<5;              //���ջ������ǿ��ж�ʹ��
	MY_NVIC_Init(3,3,UART4_IRQn,2); //��2(4����ռ(0,1,2,3)��4������(0,1,2,3))�������ռ����������ȼ�
    #endif
}


void Uart4_SendB(unsigned char ch)
{
	while((UART4->SR&0x40)==0);//�ȴ����ͽ���
	UART4->DR=ch;
}


//����4�����ַ���
void UART4_SendStr(char *s)
{
    while(*s != '\0') //����ַ���������
    {
        Uart4_SendB(*s++);//���͵�ǰ�ַ�
    }
}


void UART4_IRQHandler(void)
{
	volatile u8 res;
	res = res;
	if(UART4->SR&(1<<5))//���յ�����
	{
		res=UART4->DR;
	}
}

#endif


/*
    ����: zhiyuanTian
    ʱ��: 2018-10-21
    ���:
    1�����ӳ����ճ�ʱ��������ж�
    2������һ���ָ�������
    ʱ��: 2018-11-22
    ���:
    1�����ӽ�������볬ʱ��ʾ�ַ���
    2������1����VR���أ���1ʹ��
    ʱ��: 2018-11-22
    ���:
    1�����ӽ���ʱ�ַ��жϣ������׸��ַ�Ϊ0�����������
    �����ַ�Ϊ0�Ļ�������ַ�ȫ����Ч
    ʱ��: 2019-02-13
    ���:
    1�����ӽ��ղ��������ж�
    ʱ��: 2019-05-01
    ���:
    1�����ӽ��ո��������жϣ�����Խ������

*/
#include "stdio.h"
#include "string.h"

#ifndef _USINTERFACE_H_
#define _USINTERFACE_H_

#ifdef _USINTERFACE_GLOBALS_
#define PEXT 
#else
#define PEXT extern
#endif


#define BOOT_STR "\r -------------------------------------------------------\
                  \r|                TILL Control Terminator                |\
                  \r|                       TILL Corp                       |\
                  \r|                      Device Boot                      |\
                  \r -------------------------------------------------------\
                 "

#define RCV_TIME_OUT    800
//�������
#define CR_LF           0           //ʹ��CR-LF---1     ʹ��CR---0
#define ASCII_CR        0x0d
#define ASCII_LF        0x0a
//���ս�����־λ
#define STA_NA          0
#define STA_CR          0x2000
#define STA_LF          0x4000
#define STA_ERR         0x8000
//�����ָ���
#define C_EQUAL         '='
#define C_COMMA         ','
#define C_DASH          '-'
#define C_TERM          '\0'
#define C_COLON         ':'
#define C_SEP_1         C_DASH
#define C_SEP_2         C_COMMA
//��������������
#define P_CNT           8                            //��������
#define P_LEN           16                           //������λ����
#define W_LEN           (P_LEN*P_CNT+P_CNT-1)

#define USART_RCV_LEN   (W_LEN+1+10)                //���ջ�������С����ΪW_LEN+�ָ���+�����ֳ���
typedef struct
{
    char            *pRcvStr;  
    unsigned char   fUsIRF;                         //������ɻ��߳�ʱʱ���յ����ַ�
    char            rcvStr[USART_RCV_LEN];
    unsigned short  TimeOut;
    unsigned short  UsRxSta;
    void            (*ChRunFunc)(char*);            //��ǰ״̬Ӧ��ִ�еĹ��ܲ���
}_STR_T;

//��Ҫ�ⲿ�ӿڲ���
PEXT _STR_T str;

//��Ҫ�ⲿ�ӿں���
PEXT void BootInterface(void);
PEXT void StrProc(void);
PEXT void TimeOutInt(void);
PEXT void getSerialData(char sdata);
PEXT void int2str(int n, char *s);
PEXT int str2int(const char *s);
PEXT char tolower(char chr);
PEXT char toupper(char chr);
PEXT int strtohex(char *s);
PEXT void* memset(void* s, int c, size_t n);
PEXT char* myStrncpy(char *des, char *src, size_t n);
PEXT unsigned char FetchChar(unsigned char cmdlen, unsigned char paracnt, char *srcStr, char *para);
PEXT unsigned char FetchInt(unsigned char cmdlen, unsigned char paracnt, char *srcStr, int *para);
PEXT unsigned char UnEqFetchChar(unsigned char cmdlen, unsigned char paracnt, char *srcStr, char *para);
PEXT unsigned char UnEqFetchInt(unsigned char cmdlen, unsigned char paracnt, char *srcStr, int *para);


#undef PEXT
#endif



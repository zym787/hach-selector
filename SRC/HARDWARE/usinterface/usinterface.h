/*
    作者: zhiyuanTian
    时期: 2018-10-21
    变更:
    1、增加超接收超时清除缓存判断
    2、新增一个分隔符兼容
    时期: 2018-11-22
    变更:
    1、增加接收完成与超时显示字符串
    2、新增1条的VR开关，置1使能
    时期: 2018-11-22
    变更:
    1、增加接收时字符判断，避免首个字符为0的情况发生，
    因首字符为0的话后面的字符全部无效
    时期: 2019-02-13
    变更:
    1、增加接收参数个数判断
    时期: 2019-05-01
    变更:
    1、增加接收个数出错判断，避免越界死机

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
//命令结束
#define CR_LF           0           //使用CR-LF---1     使用CR---0
#define ASCII_CR        0x0d
#define ASCII_LF        0x0a
//接收结束标志位
#define STA_NA          0
#define STA_CR          0x2000
#define STA_LF          0x4000
#define STA_ERR         0x8000
//参数分隔符
#define C_EQUAL         '='
#define C_COMMA         ','
#define C_DASH          '-'
#define C_TERM          '\0'
#define C_COLON         ':'
#define C_SEP_1         C_DASH
#define C_SEP_2         C_COMMA
//参数长度与数量
#define P_CNT           8                            //参数个数
#define P_LEN           16                           //参数单位长度
#define W_LEN           (P_LEN*P_CNT+P_CNT-1)

#define USART_RCV_LEN   (W_LEN+1+10)                //接收缓冲区大小设置为W_LEN+分隔符+命令字长度
typedef struct
{
    char            *pRcvStr;  
    unsigned char   fUsIRF;                         //接收完成或者超时时接收到的字符
    char            rcvStr[USART_RCV_LEN];
    unsigned short  TimeOut;
    unsigned short  UsRxSta;
    void            (*ChRunFunc)(char*);            //当前状态应该执行的功能操作
}_STR_T;

//主要外部接口参数
PEXT _STR_T str;

//主要外部接口函数
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



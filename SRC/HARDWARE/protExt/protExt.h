#ifndef _PROTEXT_H_
#define _PROTEXT_H_

#ifdef _PROTEXT_GLOBALS_
#define PEXT
#else
#define PEXT extern
#endif

enum COMM
{
    HEAD_BYTE           =0xAA,  //0x5B
    LEN_DATA            =4,     //0x5B
    PROTOCOL_HEAD		=0,	    //Э��ͷ
    PROTOCOL_ADDR	    =1,	    //Э������
    PROTOCOL_COMMAND	=2,	    //Э������
    PROTOCOL_DATA		=3,	    //Э������
    PROTOCOL_CS 		=4,	    //Э��У����ֽ�
    PROTOCOL_OK		    =5,	    //Э��β
};


enum CMD
{
    SHIFT_NML           =0x01,
    SHIFT_DE            =0x02,
    SHIFT_IN            =0x03,
    ACTN_RESET          =0x05,
    WR_CUST_DATA        =0X7F,
    RD_CUST_DATA        =0XAF,
    RD_NOW_STA          =0X90,
    RD_NOW_POS          =0X99,
    SET_PROTOCAL        =0XFF,
};

#define RECEIVE_LENS        8      //�����ݷ��س���
#define REPLY_LENS          7      //�����ݷ��س���

#define rstUartBuf()        protext.rxCount = 0
#define PROT_HEAD 			protext.usartBuf[0]  		//������
#define P_ADDR 			    protext.usartBuf[1]  		//������
#define P_COMMAND 			protext.usartBuf[2]  		//������


typedef struct
{
    uint16  dataLen;
    uint8   time;
    uint8   stepCnt;
    uint8   rxCount;
    uint8   rxTimeOn;
    uint8   rxTimeCnt;
    uint8   f_RxErr;
    uint8   usartBuf[128];
    uint8   replyBuf[128];
    uint8   reply485Buf[128];
}_PROTEXT_T;
PEXT _PROTEXT_T protext;

#ifdef INFO_DEBUG
typedef struct
{
    bool    bPrint;
    uint8   cmdLast[16];
    uint8   rplyLast[16];
}_ERR_RECORD_T;
PEXT _ERR_RECORD_T errRecord;
#endif

PEXT void RxUsart(uint8 res);
PEXT void UsartProcess(void);
PEXT void CommInit(void);
PEXT void InfoErrPrint(void);


#undef PEXT
#endif



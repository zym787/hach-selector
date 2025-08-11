#ifndef _USFUNC_H_
#define _USFUNC_H_

#ifdef _USFUNC_GLOBALS_
#define PEXT
#else
#define PEXT extern
#endif

//��������ע�͵�����������
//"..."����һ�����Ա仯�Ĳ�����ʹ�ñ�����"__VA_ARGS__"�Ѳ������ݸ��ꡣ
//����ĵ���չ��ʱ��ʵ�ʵĲ����ʹ��ݸ� printf()�ˡ�
#define FSW_DBG_PRINT       1           //�����Ϣ����
#define FSW_WORK_PRINT      1           //�����Ϣ����

#if FSW_DBG_PRINT
#define LV_NA                   0
#define LV_INFO                 1
#define LV_DBG                  2
#define	printd(fmt, ...)		printf(fmt, ##__VA_ARGS__)
#define prInfo(lv,fmt, ...)     do{if(lv>=LV_INFO) printf(fmt, ##__VA_ARGS__);}while(0)
#define prDbg(lv,fmt, ...)      do{if(lv>=LV_DBG) printf(fmt, ##__VA_ARGS__);}while(0)
#else
#define printd(...)
#endif

#if FSW_WORK_PRINT
#define	printw(fmt, ... )		printf(fmt, ##__VA_ARGS__)
#else
#define printw(...)
#endif

#define S_LIST_M    "\r ---------------------Command Line----------------------\
                     \r|-/?        Display menu.                               |\
                     \r|-VR        Display software version.                   |\
                     \r|-MAP       Eg. MAP=Addr,Val.                           |\
                     \r|-IIC       Eg. IIC=Val.                                |\
                     \r|-TS        Test.                                       |\
                     \r -------------------------------------------------------\
                  "

#define S_LIST_SH   "\r --------------------Sub command Line-------------------"
#define S_LIST_SBD  "\r --------------------Sub command Body-------------------"
#define S_LIST_SE   "\r                         Yes  No                        \
                     \r -------------------------------------------------------"

#define READ_ACT            0
#define WRITE_ACT           1

#define FLASH_CHK           1                       //IIC-1 flash-0

//����ṹ��,����һ������,һ������ָ��
typedef struct
{
    unsigned char StateIndex;                       //��ǰ����������
    void (*CurrOp)(char);                           //��ǰ״̬Ӧ��ִ�еĹ��ܲ���
}_TAB_T;

typedef struct
{
    unsigned char fRwVr;                  //MAP�����д��־
    unsigned char fRwMap;                 //MAP�����д��־
    unsigned char fRwFx;                  //FX�����д��־
    unsigned char fRwI2c;                 //XP�����д��־
    unsigned char fRwTs;                  //XP�����д��־
    unsigned char fRwCat;                  //XP�����д��־
    unsigned char fRwFet;                  //XP�����д��־
    unsigned char fRwPos;                  //XP�����д��־
    unsigned char fRwFix;                  //XP�����д��־
    unsigned char fRwAddr;                  //XP�����д��־
    unsigned char fRwInt;                  //XP�����д��־
    unsigned char fRwSpd;                  //XP�����д��־
    unsigned char fRwSn;                    //XP�����д��־
    unsigned char fProtcl;                    //XP�����д��־
    unsigned char bBaudrate;                    //XP�����д��־
    unsigned char bRdPulse;                    //XP�����д��־
}_FLG_T;

#define	CMD_VR   	        "VR"
#define	CMD_LIST	        "/?"
#define	CMD_MAP	            "MAP"
#define	CMD_IIC   	        "IIC"
#define	CMD_TS	            "TS"            //VALVE PORT
#define	CMD_CAT	            "CAT"            //VALVE PORT
#define	CMD_FET	            "FET"            //VALVE PORT
#define	CMD_POS	            "POS"            //VALVE PORT
#define	CMD_FIX	            "FIX"            //VALVE PORT
#define	CMD_ADDR	        "ADDR"            //VALVE PORT

PEXT void ChRUN(char *cmdName);

#undef PEXT
#endif



#ifndef _USFUNC_H_
#define _USFUNC_H_

#ifdef _USFUNC_GLOBALS_
#define PEXT
#else
#define PEXT extern
#endif

//用于批量注释掉调试输出语句
//"..."代表一个可以变化的参数表。使用保留名"__VA_ARGS__"把参数传递给宏。
//当宏的调用展开时，实际的参数就传递给 printf()了。
#define FSW_DBG_PRINT       1           //输出信息开关
#define FSW_WORK_PRINT      1           //输出信息开关

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

//定义结构体,包含一个步骤,一个函数指针
typedef struct
{
    unsigned char StateIndex;                       //当前步骤索引号
    void (*CurrOp)(char);                           //当前状态应该执行的功能操作
}_TAB_T;

typedef struct
{
    unsigned char fRwVr;                  //MAP命令读写标志
    unsigned char fRwMap;                 //MAP命令读写标志
    unsigned char fRwFx;                  //FX命令读写标志
    unsigned char fRwI2c;                 //XP命令读写标志
    unsigned char fRwTs;                  //XP命令读写标志
    unsigned char fRwCat;                  //XP命令读写标志
    unsigned char fRwFet;                  //XP命令读写标志
    unsigned char fRwPos;                  //XP命令读写标志
    unsigned char fRwFix;                  //XP命令读写标志
    unsigned char fRwAddr;                  //XP命令读写标志
    unsigned char fRwInt;                  //XP命令读写标志
    unsigned char fRwSpd;                  //XP命令读写标志
    unsigned char fRwSn;                    //XP命令读写标志
    unsigned char fProtcl;                    //XP命令读写标志
    unsigned char bBaudrate;                    //XP命令读写标志
    unsigned char bRdPulse;                    //XP命令读写标志
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



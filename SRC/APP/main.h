#ifndef _MAIN_H_
#define _MAIN_H_

#ifdef _MAIN_H_GLOBALS_
#define PEXT
#else
#define PEXT extern
#endif

#define SOFT_VER                107
#define SOFT_VS                 "V1.0.7r8"
//#define SOFT_VS                 "V2.0.3r3"

//------------------------------------------------------------------------------------------------------------
#define ADDR_BOARD_ID           0
#define LEN_BOARD_ID            2

#define ADDR_MODULE_NUM         (ADDR_BOARD_ID+LEN_BOARD_ID)
#define LEN_MODULE_NUM          1

#define ADDR_VALVE_FIX          (ADDR_MODULE_NUM+LEN_MODULE_NUM)
#define LEN_VALVE_FIX           1

#define ADDR_DIR_FIX            (ADDR_VALVE_FIX+LEN_VALVE_FIX)
#define LEN_DIR_FIX             1

#define ADDR_PORT_CNT           (ADDR_DIR_FIX+LEN_DIR_FIX)
#define LEN_PORT_CNT            1

#define ADDR_INTVL              (ADDR_PORT_CNT+LEN_PORT_CNT)
#define LEN_INTVL               1

#define ADDR_SPD                (ADDR_INTVL+LEN_INTVL)
#define LEN_SPD                 1

#define ADDR_SN                 (ADDR_SPD+LEN_SPD)
#define LEN_SN                  5

#define ADDR_PROTOCAL           (ADDR_SN+LEN_SN)
#define LEN_PROTOCAL            1

#define ADDR_BAUD               (ADDR_PROTOCAL+LEN_PROTOCAL)
#define LEN_BAUD                1

#define ADDR_RDP               	(ADDR_BAUD+LEN_BAUD)
#define LEN_RDP                	1

//#define ADDR_SIG               	(ADDR_RDP+LEN_RDP)
//#define LEN_SIG                	32

#define ADDR_SYMBOL            	(ADDR_RDP+LEN_RDP)
#define LEN_SYMBOL            	8

#define ADDR_RDC_RATE			(ADDR_SYMBOL+LEN_SYMBOL)
#define LEN_RDC_RATE			1

#define ADDR_HALF_SEAL			(ADDR_RDC_RATE+LEN_RDC_RATE)
#define LEN_HALF_SEAL			1
//------------------------------------------------------------------------------------------------------------

#define NORMAL_BLINK            2500       //正常运行的闪烁间隔
#define RETRY_TIME_OUT          500        //异常运行的闪烁间隔

enum PROTOCAL
{
    MY_MODBUS,
    EXT_COMM
};

typedef enum
{
    PR_NONE,
    PR_INFO,
    PR_DBG,
    PR_ALL,
}_PRINT_OUT_T;

typedef struct
{
    _PRINT_OUT_T    typeInfo;
    _PRINT_OUT_T    comInfo;
    uint32  protectTimeOut;
    uint8   typeProtocal;
    uint8   bdrate;
    bool    bRdPulse;
    bool    pwrOn;
    bool    shiftOnece;     // 首次运行
    bool    reShift;        // 出错后重新走位
    bool    dbgStop;     // 首次运行
    bool    bInterrupt;
    bool    bSkipFirstSig;      // 跳过首个信号值
    uint16  sigRunTime;         // 单次电机运行时间
}_SYS_T;
PEXT _SYS_T syspara;

PEXT uint8 intCtrl;

PEXT void ParameterInit(void);
PEXT int main(void);
PEXT void DebugOut(void);
PEXT void UsrLimit(void);
PEXT void errProcRun(void);
PEXT void errActionImme(void);



#undef PEXT
#endif










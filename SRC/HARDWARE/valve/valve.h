#ifndef _VALVE_H_
#define _VALVE_H_

#ifdef _VALVE_GLOBALS_
#define PEXT
#else
#define PEXT extern
#endif


#define AXSV                0
#define SPD_LMT             255
#define LED_WORK            PCout(15)
#define VALVE_OPT           PAin(15)

#define VALVE_ENA		    PAout(4)
#define VALVE_RST		    PAout(5)
#define VALVE_DIR		    PAout(6)
#define VALVE_CLK		    PAout(7)

#define RETRY_TIMES         2                       // 重试次数
#define SIGNAL_SUM          48                      // 码盘检验次数

#define VALVE_NONE          0x00
#define VALVE_INITING       0x80
#define VALVE_RUN_END       0x40
#define VALVE_RUNNING       0x08
#define VALVE_ERR           0x20
#define OPT_ERR             0x30

//#define PULSE_CNT_EN
#define RDC01					1
#define RDC04					4
#define RDC10					10
#define RDC16					16

#ifdef A12_909_A2
#define SCALE                   64                      //当前细分数为64
#endif
#ifdef A12_906_B1
#define SCALE                   16                      //当前细分数为64
#endif
#define P_ROUND                 200                     //每圈大步数为200

#ifdef A12_909_A2
#define STEPS_1_DEGREE_RD01      (35.6)                 //每度需走的步数为12800/360=35.555~
#define STEPS_01_DEGREE_RD01     (3.6)                  //每0.1度需走的步数为12800/3600=3.555~
#define STEPS_1_DEGREE_RD04      (142.2)                 //每度需走的步数为12800/360=35.555~
#define STEPS_01_DEGREE_RD04     (14.2)                  //每0.1度需走的步数为12800/3600=3.555~
#define STEPS_1_DEGREE_RD10      (355.6)                 //每度需走的步数为12800/360=35.555~
#define STEPS_01_DEGREE_RD10     (35.6)                  //每0.1度需走的步数为12800/3600=3.555~
#define STEPS_1_DEGREE_RD16      (568.9)                 //每度需走的步数为12800/360=35.555~
#define STEPS_01_DEGREE_RD16     (56.9)                  //每0.1度需走的步数为12800/3600=3.555~
#endif
#ifdef A12_906_B1
#define STEPS_1_DEGREE_RD01      (8.9)                 //每度需走的步数为12800/360=35.555~
#define STEPS_01_DEGREE_RD01     (0.9)                  //每0.1度需走的步数为12800/3600=3.555~
#define STEPS_1_DEGREE_RD04      (35.6)                 //每度需走的步数为12800/360=35.555~
#define STEPS_01_DEGREE_RD04     (3.6)                  //每0.1度需走的步数为12800/3600=3.555~
#define STEPS_1_DEGREE_RD10      (88.9)                 //每度需走的步数为12800/360=35.555~
#define STEPS_01_DEGREE_RD10     (8.9)                  //每0.1度需走的步数为12800/3600=3.555~
#define STEPS_1_DEGREE_RD16      (142.2)                 //每度需走的步数为12800/360=35.555~
#define STEPS_01_DEGREE_RD16     (14.2)                  //每0.1度需走的步数为12800/3600=3.555~
#endif

#define BYTE_RANGE              255
#define ADDR_MAX                64

#define SPD_VALVE               28                     // 转阀速度

#define PREPORTCNT              4                       // 预输入端口
typedef struct
{
    unsigned char Addr;             //模块地址
    unsigned char status;           //通阀当前状态
    unsigned char dir;              //寻位方向
    unsigned char retryTms;         //走位重试的次数

    unsigned char initStep;         //阀初始化的步骤
    unsigned char portCur;          //通阀当前位置编号
    unsigned char portDes;          //通阀目标位置编号
    unsigned char limitSignal;      //
    
    unsigned char statusLast;       //
    unsigned char passByOne;        //重新校准1号位标志
    unsigned char bErrRetn;         //
    unsigned char threshSignal;     // 信号阈值个数
    
    unsigned char bErr;             // 出错标志
    unsigned char bHalfSeal;             // 出错标志
    short         cntSignal;        // 信号个数

    unsigned char bNewInit;         // 刚复位完成
    unsigned short ErrBlinkTime;    //错误灯提示间隔
    unsigned char fixOrg;          	// 原点补偿
    unsigned char spd;              // 再次复位转动

    unsigned int OptGap;            //光耦高电平脉冲步数
    unsigned int OptBlock;          //光耦低电平脉冲步数
    unsigned int nowGapCnt;         //光耦低电平脉冲步数
    unsigned int nowBlockCnt;       //光耦低电平脉冲步数
    unsigned int stpCnt;            //初始化后开始补偿的步数

    unsigned int BaudRate;          //运行的波特率值
    unsigned char SnCode[LEN_SN];   // 序列码
    unsigned char serialNum;
    unsigned char serialPort[PREPORTCNT];
    unsigned char bGetOrg;
}_VALVE_T;
PEXT _VALVE_T Valve;


typedef struct
{
	uint8	rate;
	int	    stepRound;
	float	stepP1dgr;
	float	stepP01dgr;
}RDC_T;
PEXT RDC_T rdc;

#define FIX_VAL_LENGHT      14
typedef union
{
    unsigned char array[FIX_VAL_LENGHT];
    struct
    {
        unsigned char port1Val;
        unsigned char port2Val;
        unsigned char port3Val;
        unsigned char port4Val;

        unsigned char port5Val;
        unsigned char port6Val;
        unsigned char port7Val;
        unsigned char port8Val;

        unsigned char port9Val;
        unsigned char port10Val;
        unsigned char port11Val;
        unsigned char port12Val;

        unsigned char dirGap;          //
        unsigned char portCnt;          //
    }fix;
}_12VALVE_FIX;
PEXT _12VALVE_FIX valveFix;

PEXT void ConfigValve(void);
PEXT void InitValve(void);
PEXT void ValveProcess(void);
PEXT void ProcessValve(void);
PEXT void ValveLimitDetect(void);
PEXT void ProcessInterrupt(void);
PEXT void TestBurn(void);

#undef PEXT
#endif



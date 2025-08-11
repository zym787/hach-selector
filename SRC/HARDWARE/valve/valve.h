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

#define RETRY_TIMES         2                       // ���Դ���
#define SIGNAL_SUM          48                      // ���̼������

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
#define SCALE                   64                      //��ǰϸ����Ϊ64
#endif
#ifdef A12_906_B1
#define SCALE                   16                      //��ǰϸ����Ϊ64
#endif
#define P_ROUND                 200                     //ÿȦ����Ϊ200

#ifdef A12_909_A2
#define STEPS_1_DEGREE_RD01      (35.6)                 //ÿ�����ߵĲ���Ϊ12800/360=35.555~
#define STEPS_01_DEGREE_RD01     (3.6)                  //ÿ0.1�����ߵĲ���Ϊ12800/3600=3.555~
#define STEPS_1_DEGREE_RD04      (142.2)                 //ÿ�����ߵĲ���Ϊ12800/360=35.555~
#define STEPS_01_DEGREE_RD04     (14.2)                  //ÿ0.1�����ߵĲ���Ϊ12800/3600=3.555~
#define STEPS_1_DEGREE_RD10      (355.6)                 //ÿ�����ߵĲ���Ϊ12800/360=35.555~
#define STEPS_01_DEGREE_RD10     (35.6)                  //ÿ0.1�����ߵĲ���Ϊ12800/3600=3.555~
#define STEPS_1_DEGREE_RD16      (568.9)                 //ÿ�����ߵĲ���Ϊ12800/360=35.555~
#define STEPS_01_DEGREE_RD16     (56.9)                  //ÿ0.1�����ߵĲ���Ϊ12800/3600=3.555~
#endif
#ifdef A12_906_B1
#define STEPS_1_DEGREE_RD01      (8.9)                 //ÿ�����ߵĲ���Ϊ12800/360=35.555~
#define STEPS_01_DEGREE_RD01     (0.9)                  //ÿ0.1�����ߵĲ���Ϊ12800/3600=3.555~
#define STEPS_1_DEGREE_RD04      (35.6)                 //ÿ�����ߵĲ���Ϊ12800/360=35.555~
#define STEPS_01_DEGREE_RD04     (3.6)                  //ÿ0.1�����ߵĲ���Ϊ12800/3600=3.555~
#define STEPS_1_DEGREE_RD10      (88.9)                 //ÿ�����ߵĲ���Ϊ12800/360=35.555~
#define STEPS_01_DEGREE_RD10     (8.9)                  //ÿ0.1�����ߵĲ���Ϊ12800/3600=3.555~
#define STEPS_1_DEGREE_RD16      (142.2)                 //ÿ�����ߵĲ���Ϊ12800/360=35.555~
#define STEPS_01_DEGREE_RD16     (14.2)                  //ÿ0.1�����ߵĲ���Ϊ12800/3600=3.555~
#endif

#define BYTE_RANGE              255
#define ADDR_MAX                64

#define SPD_VALVE               28                     // ת���ٶ�

#define PREPORTCNT              4                       // Ԥ����˿�
typedef struct
{
    unsigned char Addr;             //ģ���ַ
    unsigned char status;           //ͨ����ǰ״̬
    unsigned char dir;              //Ѱλ����
    unsigned char retryTms;         //��λ���ԵĴ���

    unsigned char initStep;         //����ʼ���Ĳ���
    unsigned char portCur;          //ͨ����ǰλ�ñ��
    unsigned char portDes;          //ͨ��Ŀ��λ�ñ��
    unsigned char limitSignal;      //
    
    unsigned char statusLast;       //
    unsigned char passByOne;        //����У׼1��λ��־
    unsigned char bErrRetn;         //
    unsigned char threshSignal;     // �ź���ֵ����
    
    unsigned char bErr;             // �����־
    unsigned char bHalfSeal;             // �����־
    short         cntSignal;        // �źŸ���

    unsigned char bNewInit;         // �ո�λ���
    unsigned short ErrBlinkTime;    //�������ʾ���
    unsigned char fixOrg;          	// ԭ�㲹��
    unsigned char spd;              // �ٴθ�λת��

    unsigned int OptGap;            //����ߵ�ƽ���岽��
    unsigned int OptBlock;          //����͵�ƽ���岽��
    unsigned int nowGapCnt;         //����͵�ƽ���岽��
    unsigned int nowBlockCnt;       //����͵�ƽ���岽��
    unsigned int stpCnt;            //��ʼ����ʼ�����Ĳ���

    unsigned int BaudRate;          //���еĲ�����ֵ
    unsigned char SnCode[LEN_SN];   // ������
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



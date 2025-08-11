#ifndef _MODBUS_H_
#define _MODBUS_H_

#ifdef _MODBUS_GLOBALS_
#define PEXT
#else
#define PEXT extern
#endif

#define DEBUG_MODBUS                0

//======================  �����궨��=================================
#define MAX_ADDR                    1
#define	MB_SALVE_DEF_ADDR           1
//---- ����֧��MODBUS���ܴ���-------------------------------------------------
#define CHECK_COIL_bSTATUS			1 		//  �����λ״̬
#define GET_INPUT_bSTATUS			2 		//  ������λ״̬
#define GET_HOLDING_REGT			3		//  �����ּĴ���
#define GET_INPUT_REGT				4		//  ������Ĵ���
#define SET_COIL_SINGLE				5		//  ǿ�õ���Ȧ
#define PRESET_HOLDING_sREGT		6		//  Ԥ�õ������ּĴ���
#define SET_COIL_MULTIPLE			15		//  ǿ�ƶ��λ���
#define PRESET_HOLDING_mREGT		16		//  Ԥ�ö�����ּĴ���
#define EXTERN_FUNC			        0x52    //  ��չ����
//---- �������Ĵ���ֵ-------------------------------------------------
#define ERR_NOT						0
#define ERR_MB_FUN					1		// ���Ϸ����ܴ���
#define ERR_MB_ADDR					2		// ���Ϸ����ݵ�ַ
#define ERR_MB_DATA					3		// ���Ϸ����ݣ�����ĳ���ܵļ���ֵ
#define ERR_MB_DEVICE				4		// ��ģʽ��Ӧ�����ȴ�ʱ�䣻��ģʽ�豸��ַ���������ַ����ʱ�� ����1.5���ַ�
//---- ͨ��ʱ�����-------------------------------------------------
#define ONE_DATA_BIT				10		// ���÷�ʽ1:10bit
#define BAUD_RATE_9600			    9600L	// MODBUS������
#define BAUD_RATE_19200			    19200L	// MODBUS������
#define BAUD_RATE_MODBUS			19200L	// MODBUS������
//ÿ��bit��ʱ��Ϊ1/BR��ÿ���ֽ�Ϊ8λ+ֹͣλ+У��λ=10λ����ÿ���ֽں�ʱ10/BR�����ֽ�Ϊ5/BR��ת��Ϊus����1000000
#define MODBUS_TIME_9600			(520)		// 520uS=(10/9600)/2
#define MODBUS_TIME_19200			(260)		// 260uS=(10/19200)/2
#define MODBUS_TIME					(10*1000000/BAUD_RATE_MODBUS/2)		// 260uS=(10/19200)/2
#define BUS_IDLE_TIME				10		// ֡����(��ʼ�����)����3.5~4�������ַ�ʱ��
#define FRAME_ERR_TIME				4		// ���ݽ��ճ�ʱ,�����ַ�֮�� ����1.5~2�������ַ�ʱ��
#define NO_RESPONSE_TIME			500		// ͨѶ�жϳ�ʱ Լ500*MODBUS_TIME
#define NO_RESPONSE_TIME_BC			500		// ͨѶ�жϳ�ʱ Լ500*MODBUS_TIME
//---- ͨ��״̬����-------------------------------------------------
#define MB_IDEL						0X00	// ���߿��С�����֡����ʼ֡
#define MB_WAIT_HOST				0X01	// ���豸�ȴ���Ӧ
#define MB_BUSY						0X50	// ����æ
#define MB_RECIVE					0X51	// ���߽���
#define MB_SEND						0X52	// ���߷���
#define MB_RECIVE_END				0X53	// ���߷���
#define MB_NO_RESPONSE				0X54	// ��ģʽ��Ӧ�ĵ�ǰ��ѯ���豸����ģʽ�豸��ַ����
#define MB_RECIVE_ERR				0X55	// ���߽��չ����г���
//---- ͨ�ŵ�ַ�������˴��ٸ�?-------------------------------------------------
#define MB_Broadcast_ADDR			0xaa	// �㲥��ַ

#define MB_MAX_ADDR					128		// ģ������ַ

#define MB_REPEAT					2		// ���ݴ�����������´���Ĵ���
#define MB_ADDR_ERR_FLAG			0xFF	// ��ַ������Ч���
#define MB_ADDR_NULL				0xFF	// ģ�鲻����

//=====================�����ͱ�������=============================
//Э�鴫�����
#define LENGTH_MB_DATA				140	    // 11*12 +4*2
#define LENGTH_MB_PARA				288	    // 140*2+8
#define LEAST_RCV_CNT               3       // ��С�����ֽ���
typedef struct
{
	unsigned char 	sRUN;					// ��������״̬
	unsigned char 	sERR;					// ���߳���״̬
	unsigned char 	rCnt;					// ���߽��ռ���
	unsigned char 	HostRept;				// ��Ϊ���豸���ظ�����
	unsigned short 	times;					// ���߼�ʱ1
	unsigned char	mAddrs;					// �豸��ַ
	unsigned char	res;				    // ����---��2�ı���
	unsigned char	rBuf[LENGTH_MB_DATA];	// �������Ľ�������
	unsigned char	tBuf[LENGTH_MB_DATA];	// ����������
}_MB_PRAR_T;


//��Ȧ����
#define SUM_COIL_BIT				64		// ��Ȧ��λ����
#define LENGTH_COIL_REG			    SUM_COIL_BIT/8

//��ɢ���������
#define SUM_DiscreteREG_BIT			32		// ��ɢ����λ����
#define LENGTH_DiscreteREG			SUM_DiscreteREG_BIT	/8

//���ּĴ�������
#define SUM_HoldingREG_WORD		    120		// ���ּĴ����ĸ���
#define LENGTH_HoldingREG			SUM_HoldingREG_WORD*2

//����Ĵ�������
#define SUM_InputREG_WORD			20		// ���ּĴ����ĸ���
#define LENGTH_InputREG				SUM_HoldingREG_WORD*2

//�û���������������Ĵ�����ʹ�ù���
#define COIL_NUM01				    0
#define DisREG_NUM01			    (COIL_NUM01 + SUM_COIL_BIT)
#define HoldingREG01				0
#define InputREG01				    (HoldingREG01 + SUM_HoldingREG_WORD)

//HMI��ͨѶ�ֵ�ַ����
// ���ּĴ���
#define 	HMI_hREG01_BasePic			0							// �����ַ
#define 	HMI_hREG02_CMD				1							// ���������ַ

// ����Ĵ���

// HMI��ͨѶλ��ַ����
// ��Ȧ����: 0~15it
#define	HMI_COIL_BIT0		0x01	//
#define	HMI_COIL_BIT1		0x02	//
#define	HMI_COIL_BIT2		0x04	//
#define	HMI_COIL_BIT3		0x08	//
#define	HMI_COIL_BIT4		0x10	//
#define	HMI_COIL_BIT5		0x20	//
#define	HMI_COIL_BIT6		0x40	//
#define	HMI_COIL_BIT7		0x80	//

//��ɢ�������:16~71 bit : 16~23 [0]  24`31[1] 32~39 [2] 40~47 [3] 48~55 [4] 56~63 [5] 64~71 [6]
#define	HMI_DisIN_BIT0		0x01	//
#define	HMI_DisIN_BIT1		0x02	//
#define	HMI_DisIN_BIT2		0x04	//
#define	HMI_DisIN_BIT3		0x08	//
#define	HMI_DisIN_BIT4		0x10	//
#define	HMI_DisIN_BIT5		0x20	//
#define	HMI_DisIN_BIT6		0x40	//
#define	HMI_DisIN_BIT7		0x80	//

//----��������----------------
PEXT _MB_PRAR_T	    ModbusPara;
PEXT unsigned char 	CoilBitPara[LENGTH_COIL_REG];
PEXT unsigned char 	DiscreteBitPara[LENGTH_DiscreteREG];
PEXT unsigned char 	HoldingREGPara[LENGTH_HoldingREG];
PEXT unsigned char 	InputREGPara[LENGTH_InputREG];

//----��������----------------
PEXT unsigned short  ModbusCRC16( unsigned char  *puchMsg, unsigned short usDataLen) ;
PEXT void ModbusInit(void);
PEXT void ModbusSend(unsigned char length) ;
PEXT void ModbusReceive(unsigned char res);
PEXT void ModbusTimesProcess(void);
PEXT void Modbus_ERROR(void) ;
PEXT void MB_ReadHoldingRegisters(void) ;
PEXT void MB_PresetSingleHoldingRegister(void);
PEXT void MB_PresetMultipleHoldingRegisters(void);
PEXT void ModbusProces(void);
PEXT void ModbusRequest(unsigned char style, unsigned char sAdd,unsigned short val);

#undef PEXT
#endif



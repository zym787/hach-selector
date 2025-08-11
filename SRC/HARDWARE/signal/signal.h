#ifndef _SIGNAL_H_
#define _SIGNAL_H_

#ifdef _SIGNAL_GLOBALS_
#define PEXT
#else
#define PEXT extern
#endif

#define PERCENT_TOLL    5
#define PERCENT         100

#define SIGNAL_CNT          24

typedef struct
{
    unsigned short pulseGap[SIGNAL_CNT];            // high pulse
    unsigned short pulseBlock[SIGNAL_CNT];          // low pulse
    unsigned short basicPulse;                      // ���һ���źŵ�ͨ����ɵ��������
    unsigned short pulse[SIGNAL_CNT];               // ���һ���źŵ�ͨ����ɵ��������
    bool            bRdPulse;
    unsigned char num;                              // �ź���ֵ���
    unsigned char bEnPulse;                         // ȡ���һ���źſ�ʼ��ͨ���������־
    unsigned char stpScan;                          // �ź�ɨ���־
    unsigned char sum;                              // �ź���ֵ����
    unsigned char count;                            // �źŸ���
    unsigned char scanCount;                        // �źŸ���
    unsigned char arrCount[SIGNAL_CNT];             // ɨ�����ͨ���źŸ���
}SIGNAL_T;
PEXT SIGNAL_T sig;


PEXT void getOptStartStatus(void);
PEXT bool GettCliffSignal(void);
PEXT void SignalScan(void);
PEXT uint8 SigSum(uint8 *array, uint8 len);

#undef PEXT
#endif



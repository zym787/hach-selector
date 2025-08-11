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
    unsigned short basicPulse;                      // 最后一个信号到通道完成的脉冲个数
    unsigned short pulse[SIGNAL_CNT];               // 最后一个信号到通道完成的脉冲个数
    bool            bRdPulse;
    unsigned char num;                              // 信号阈值编号
    unsigned char bEnPulse;                         // 取最后一个信号开始到通道的脉冲标志
    unsigned char stpScan;                          // 信号扫描标志
    unsigned char sum;                              // 信号阈值个数
    unsigned char count;                            // 信号个数
    unsigned char scanCount;                        // 信号个数
    unsigned char arrCount[SIGNAL_CNT];             // 扫描各个通道信号个数
}SIGNAL_T;
PEXT SIGNAL_T sig;


PEXT void getOptStartStatus(void);
PEXT bool GettCliffSignal(void);
PEXT void SignalScan(void);
PEXT uint8 SigSum(uint8 *array, uint8 len);

#undef PEXT
#endif



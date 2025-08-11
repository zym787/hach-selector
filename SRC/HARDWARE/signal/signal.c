#define _SIGNAL_GLOBALS_
#include "common.h"

static bool optStaLst=false;
void getOptStartStatus(void)
{
    optStaLst = (bool)VALVE_OPT;
}

/*

*/
bool GettCliffSignal(void)
{
    bool bCliff=false;

    if(optStaLst==(bool)VALVE_OPT)
    {
        return false;
    }
    if(VALVE_OPT)
    {// ��Ƭ
        if(optStaLst==false)
        {
            prInfo(syspara.typeInfo, "\r\n gap %d", Valve.nowGap);
            if(Valve.nowGap>(sig.pulseGap[0]-sig.pulseGap[0]/3) && Valve.nowGap<(sig.pulseGap[1]*2))
            {
//                prInfo(syspara.typeInfo, " %d", Valve.nowGapCnt);
                Valve.nowGap = 0;
                bCliff = true;
            }
            else
            {
                prInfo(syspara.typeInfo, " err %d %d %d", Valve.nowGap, (sig.pulseGap[0]-sig.pulseGap[0]/3), (sig.pulseGap[1]*2));
            }
        }
    }
    else
    {
        if(optStaLst==true)
        {
            prInfo(syspara.typeInfo, "\r\n block %d", Valve.nowBlock);
            if(Valve.nowBlock>(sig.pulseBlock[1]-sig.pulseBlock[1]/2) && Valve.nowBlock<(sig.pulseBlock[0]*2))
            {
//                prInfo(syspara.typeInfo, " %d", Valve.nowBlockCnt);
                Valve.nowBlock = 0;
                bCliff = true;
            }
            else 
            {
                if(Valve.cntSignal)
                {
                    Valve.status = OPT_ERR;
                    prInfo(syspara.typeInfo, " err %d %d %d", Valve.nowBlock, (sig.pulseBlock[1]-sig.pulseBlock[1]/3), (sig.pulseBlock[0]*2));
                }
                else
                {// ��һ���������ڰ�ݿ�ʼ������ֵ��С����ֵ��������
                    bCliff = true;
                }
            }
        }
    }
    optStaLst = (bool)VALVE_OPT;
    return bCliff;
}

/*
    ��������
*/
void BubbleWord(uint16* array,uint8 len)
{
	uint8 i,j;
	uint16 temp;
	for(j=1;j<len;j++)
	{
		for(i=0;i<len-j;i++)
		{
			if(array[i]<array[i+1])
			{
				temp=array[i];
				array[i]=array[i+1];
				array[i+1]=temp;
			}
		}
	}
}


/*
    ��ֵ
*/
uint16 AverageN(uint16* array,uint8 len)
{
	uint8 i=0;
	uint32 sum=0;
	for(i=0;i<len;i++)
	{
        sum += *(array+i);
	}
    sum /= len;
    return sum;
}


/*

*/
uint8 SigSum(uint8 *array, uint8 len)
{
    uint8 sum=0;
    for(uint8 i=0; i<len; i++)
        sum += *(array+i);
    return sum;
}


uint16 SigSumU16(uint16 *array, uint8 len)
{
    uint16 sum=0;
    for(uint8 i=0; i<len; i++)
        sum += *(array+i);
    return sum;
}

/*
    ɨ��ͨ�����źŷ���
*/
void SignalScan(void)
{
    uint8 i=0;
    uint8 rwBuff[LEN_SYMBOL]={0,0,0,0,0,0,0,0};
    uint32 temp=0;
    switch(sig.stpScan)
    {
        case 100:
            if(MotionStatus[AXSV])
            {
                srd[AXSV].accel_count = srd[AXSV].decel_val;
                srd[AXSV].run_state = DECEL;
                sig.stpScan = 101;
            }
            else
            {
                sig.stpScan = 101;
            }
            break;
        case 101:
            if(!MotionStatus[AXSV])
            {
                VALVE_ENA = ON;
                sig.bRdPulse = true;
                sig.count = 0;
                AxisMoveRel(AXSV, -(int)rdc.stepRound, accel[AXSV]/2, decel[AXSV]/2, speed[AXSV]/2);
                sig.stpScan = 102;
            }
            break;
        case 102:
            if(MotionStatus[AXSV])
            {
                sig.stpScan = 103;
            }
            break;
        case 103:
            if(!MotionStatus[AXSV])
            {
                prInfo(syspara.typeInfo, "\r\n �ź�����:%d", sig.count);
                if(sig.count!=24)
                    prInfo(syspara.typeInfo, "\r\n �ź���������");
                VALVE_ENA = ON;
                sig.bRdPulse = true;
                sig.blockNum = 0;
                sig.gapNum = 0;
                memset(sig.pulseGap, 0, sizeof(sig.pulseGap));
                memset(sig.pulseBlock, 0, sizeof(sig.pulseBlock));
                AxisMoveRel(AXSV, -(int)rdc.stepRound*13/12, accel[AXSV]/2, decel[AXSV]/2, speed[AXSV]/2);
                sig.stpScan = 104;
            }
            break;
        case 104:
            if(!MotionStatus[AXSV])
            {
                sig.bRdPulse = false;
                BubbleWord(sig.pulseGap, SIGNAL_CNT);
                BubbleWord(sig.pulseBlock, SIGNAL_CNT);
                prInfo(syspara.typeInfo, "\r\nG:");
                for(i=0; i<SIGNAL_CNT; i++)
                {
                    prInfo(syspara.typeInfo, " %d", *(sig.pulseGap+i));
                    if(!*(sig.pulseGap+i))
                        break;
                }
                // ȡ��ֵ
                sig.pulseGap[i-1] = AverageN(sig.pulseGap+1, i-1);
                
                prInfo(syspara.typeInfo, "\r\nB:");
                for(i=0; i<SIGNAL_CNT; i++)
                {
                    prInfo(syspara.typeInfo, " %d", *(sig.pulseBlock+i));
                    if(!*(sig.pulseBlock+i))
                        break;
                }
                // ȡ��Сֵ�����ڶ�����һ�㱣֤ɨ��һȦ��С�ݿ��ܻ�ɨ�����Σ������ȡ�����ֵ
                sig.pulseBlock[0] = sig.pulseBlock[i-3];    // AverageN(sig.pulseBlock, i-1);
                if(sig.pulseBlock[0]<sig.pulseGap[i-1])
                {
                    prInfo(syspara.typeInfo, "\r\n %d %d", sig.pulseBlock[0], sig.pulseGap[i-1]);
                    prInfo(syspara.typeInfo, "\r\n �źŷ���,���������ϵ�");
                    while(1);
                }
                else if(sig.pulseGap[0]<sig.pulseGap[i-1]*3/2 || sig.pulseBlock[0]<sig.pulseBlock[i-1]*3/2)
                {
                    prInfo(syspara.typeInfo, "\r\n ɨ���쳣������ɨ��");
                    prInfo(syspara.typeInfo, "\r\n %d %d %d %d", sig.pulseGap[0], sig.pulseGap[i-1], sig.pulseBlock[0], sig.pulseBlock[i-1]);
                    sig.stpScan = 100;
                    break;
                }
                rwBuff[0] = sig.pulseBlock[0]>>8;
                rwBuff[1] = sig.pulseBlock[0];
                rwBuff[2] = sig.pulseBlock[i-1]>>8;
                rwBuff[3] = sig.pulseBlock[i-1];
                rwBuff[4] = sig.pulseGap[i-1]>>8;
                rwBuff[5] = sig.pulseGap[i-1];
                rwBuff[6] = sig.pulseGap[0]>>8;
                rwBuff[7] = sig.pulseGap[0];

                prInfo(syspara.typeInfo, "\r\n ����� %d, С��%d, �����%d, ���%d",
                sig.pulseBlock[0], sig.pulseBlock[i-1], sig.pulseGap[i-1], sig.pulseGap[0]);
                I2CPageWrite_Nbytes(ADDR_SYMBOL, LEN_SYMBOL, rwBuff);
                sig.stpScan = 1;
            }
            break;
        case 1:
            prInfo(syspara.typeInfo, "\r\n ���㲢�洢����ֵ");
            I2CPageRead_Nbytes(ADDR_SYMBOL, LEN_SYMBOL, rwBuff);
            // ��������B0Ϊ����ݣ�B1ΪС�ݣ�B2Ϊ��������ֵ��B3ΪС�����ֵ��G0Ϊ����ڣ�G1Ϊ��ڣ�G2Ϊ�������G3Ϊ������
            sig.pulseBlock[0] = rwBuff[0];
            sig.pulseBlock[0] <<= 8;
            sig.pulseBlock[0] |= rwBuff[1];
            // lit blade 20 percent
            temp = sig.pulseBlock[0]*PERCENT_TOLL;
            sig.pulseBlock[2] = temp/PERCENT;

            sig.pulseBlock[1] = rwBuff[2];
            sig.pulseBlock[1] <<= 8;
            sig.pulseBlock[1] |= rwBuff[3];
            // large nick 20 percent
            temp = sig.pulseBlock[1]*PERCENT_TOLL;
            sig.pulseBlock[3] = temp/PERCENT;

            sig.pulseGap[0] = rwBuff[4];
            sig.pulseGap[0] <<= 8;
            sig.pulseGap[0] |= rwBuff[5];
            // normal Gap 20 percent
            temp = sig.pulseGap[0]*PERCENT_TOLL;
            sig.pulseGap[2] = temp/PERCENT;
            
            sig.pulseGap[1] = rwBuff[6];
            sig.pulseGap[1] <<= 8;
            sig.pulseGap[1] |= rwBuff[7];
            // normal Gap 20 percent
            temp = sig.pulseGap[1]*PERCENT_TOLL;
            sig.pulseGap[3] = temp/PERCENT;
            sig.stpScan = 2;
            break;
        case 2:
            prInfo(syspara.typeInfo, "\r\n ɨ�����������λ");
            VALVE_ENA = ON;
            Valve.status = VALVE_INITING;
            syspara.pwrOn = true;
            Valve.ErrBlinkTime = NORMAL_BLINK;
            Valve.stpCnt = 0;
            Valve.portDes = 0;
            Valve.passByOne = 0;
            Valve.retryTms = 0;
            syspara.protectTimeOut = 0;
            Valve.initStep = 0;
            Valve.stpCnt = 0;
            sig.stpScan = 0;
            break;
        default:
            break;
    }
}






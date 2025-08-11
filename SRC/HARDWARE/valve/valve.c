#define _VALVE_GLOBALS_
#include "common.h"

/*
    ����12λ�ľ������꣬���ź�Ϊ��������һ��ƫ������ԭ�㲹������
    �ﵽ�źŸ����󰴴�ƫ�������м��٣����ض���Ϊ��ǰλ������
    ʹλ�õ���������ȶ��������ź�����׼��������������Ĳ�����ͬ
*/
void ConfigValve(void)
{
    RCC->APB2ENR |= (RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC);

    //LED
    GPIOC->CRH &= (GPIO_Crh_P15);
    GPIOC->CRH |= (GPIO_Mode_Out_PP_50MHz_P15);
    //VALVE OPTO
    GPIOA->CRH &= (GPIO_Crh_P15);
    GPIOA->CRH |= (GPIO_Mode_IN_PU_PD_P15);
    GPIOA->ODR |= (GPIO_Pin_15);

    GPIOA->CRL &= (GPIO_Crl_P4 & GPIO_Crl_P5 & GPIO_Crl_P6 & GPIO_Crl_P7);
    GPIOA->CRL |= (GPIO_Mode_Out_PP_50MHz_P4 | GPIO_Mode_Out_PP_50MHz_P5 | GPIO_Mode_Out_PP_50MHz_P6 | GPIO_Mode_Out_PP_50MHz_P7);
    VALVE_RST = 0;
    delay_ms(10);
    VALVE_RST = 1;

    //-----------------------------------------------------------------------------
    srd[AXSV].SearchOrg = ValveLimitDetect;
    srd[AXSV].bEmgStop = NULL;
    //-----------------------------------------------------------------------------
    srd[AXSV].signalCLK = &PAout(7);
    srd[AXSV].signalDIR = &PAout(6);

    srd[AXSV].signalCCR1= &TIM4->CCR1;
    srd[AXSV].signalARR = &TIM4->ARR;
    srd[AXSV].signalCR1= &TIM4->CR1;
}

/*

*/
void InitValve(void)
{
    if(Valve.status&VALVE_INITING && !MotionStatus[AXSV])
    {
        {
            if(Valve.retryTms<RETRY_TIMES)
            {
                ++Valve.retryTms;
                prInfo(syspara.typeInfo, "\r\n �������");
                VALVE_ENA = ENABLE;
                position[AXSV] = HomePos[AXSV];     // �����ǰ�ļƲ�
                AxisMoveAbs(AXSV, -rdc.stepRound*3, accel[AXSV], decel[AXSV], speed[AXSV]);
                Valve.status |= VALVE_RUNNING;                  // ��λ���б�־
            }
            else
            {
                Valve.ErrBlinkTime = RETRY_TIME_OUT;
                Valve.status = VALVE_ERR;
                VALVE_ENA = DISABLE;
                prInfo(syspara.typeInfo, "\r\n �ظ�λʧ��");
            }
        }
    }
}

void getPrePort(void)
{
    if(Valve.serialPort[0]||Valve.serialPort[1]||Valve.serialPort[2]||Valve.serialPort[3])
    {
        if(Valve.serialPort[0])
        {
            Valve.portDes = Valve.serialPort[0];
            Valve.serialPort[0] = 0;
        }
        else if(Valve.serialPort[1])
        {
            Valve.portDes = Valve.serialPort[1];
            Valve.serialPort[1] = 0;
        }
        else if(Valve.serialPort[2])
        {
            Valve.portDes = Valve.serialPort[2];
            Valve.serialPort[2] = 0;
        }
        else if(Valve.serialPort[3])
        {
            Valve.portDes = Valve.serialPort[3];
            Valve.serialPort[3] = 0;
        }
    }
    else
    {
        Valve.serialNum = 0;
    }
}

/*
    ����λ���������Ҫ�߶��ٲ���
    �����ǰ��ź�����λ����Ҫ���ߣ��˴����߰��ͨ��
    ���ź�����������֮�����жϴ�������ֵ���м��ٶ�������֤ÿ�ε����Ŀ��λ�ö����źŵ㸽��
*/
int getCoord(sint32 destiny)
{
    sint32 dCoord=0;
    dCoord = destiny;
    dCoord *= 360/valveFix.fix.portCnt;
    dCoord += 180/valveFix.fix.portCnt;
    dCoord *= rdc.stepP1dgr;
    return dCoord;
}



/*
    ������ֵ������ٲ���
*/
void DeccelStep(uint8 dir)
{
    if(dir==CW)
    {
        srd[AXSV].accel_count = -Valve.fixOrg*rdc.stepP01dgr;
    }
    else
    {
        srd[AXSV].accel_count = -Valve.fixOrg*rdc.stepP01dgr;
    }
    // ������ٶ�
    srd[AXSV].run_state = DECEL;
}

/*
    ��ʼ����ɺ����ͨ��Ѱλ��Ԥ������Ȧ���г̣��϶����ҵ�����Ŀ��λ��
    �ҵ�һ��ͨ���㣬λ�ü�1��ֱ���ҵ�Ŀ��λ�ã����ͣ
    �������һȦû���ҵ�Ŀ��λ�ã��������³�ʼ�������³�ʼ���Ĵ�������3�Σ������˳�
*/
void ProcessValve(void)
{
    int Positive=0, Negative=0, sub=0, coord=0;     // �����뷴���ֵ
    if(!(Valve.status&VALVE_INITING))
	{
        if(!MotionStatus[AXSV])
        {
            if(Valve.status==VALVE_RUN_END)
        	{
                if(Valve.portDes)
                {
                    if(Valve.portCur!=Valve.portDes && Valve.portDes<=valveFix.fix.portCnt)
            	    {
                        if(Valve.dir==0xff)
                        {// �ͽ�ԭ����λ
                            // �ҳ��������
                            if(Valve.portCur==0xff)
                            {
                                Valve.portCur = 0;
                            }

                            // ����ǰ����������ǰλ����Ŀ��λ�þ�����ٸ���λ
                            // �ͽ�ԭ����ȡСֵ���Ǹ�����
                            if(Valve.portDes>Valve.portCur)
                            {// Ŀ��λ���ڵ�ǰλ�ú���
                                Positive = Valve.portDes-Valve.portCur;
                                Negative = Valve.portCur-Valve.portDes+valveFix.fix.portCnt;
                            }
                            else
                            {// Ŀ��λ���ڵ�ǰλ��ǰ��
                                Positive = Valve.portDes-Valve.portCur+valveFix.fix.portCnt;
                                Negative = Valve.portCur-Valve.portDes;
                            }
                            if(Positive<Negative)
                            {
                                prDbg(syspara.typeInfo, "\r\n CCW-");
                                if(position[AXSV]>=0)
                                {
                                    if(Valve.portDes<=Positive)     // ��ԭ��
                                    {
                                        sub = Valve.portDes;
                                        prDbg(syspara.typeInfo, "1 <%d>", sub);
                                    }
                                    else
                                    {
                                        sub = Valve.portDes-valveFix.fix.portCnt;
                                        prDbg(syspara.typeInfo, "2 <%d>", sub);
                                    }
                                }
                                else
                                {
                                    sub = Valve.portCur+Positive;
                                    prDbg(syspara.typeInfo, "3 <%d>", sub);
                                }
                                // ����λ���������Ҫ�������źŸ���
                                Valve.limitSignal = Positive*SIGNAL_SUM/valveFix.fix.portCnt;
                                // ����ת���ʱ�򣬵�ǰ�ǰ�ͨ��λ�õ�ʱ����Ҫ�۳�2���ź�ֵ
                                if(Valve.bHalfSeal && !Valve.portCur)
                                {
                                    Valve.limitSignal -= SIGNAL_SUM/valveFix.fix.portCnt/2;
                                }
                                coord = getCoord(Positive);
                                AxisMoveRel(AXSV, -coord, accel[AXSV], decel[AXSV], speed[AXSV]);
                            }
                            else if(Positive>=Negative)
                            {
                                prDbg(syspara.typeInfo, "\r\n CW-");
                                if(Valve.portCur>Negative)
                                {
                                    if(position[AXSV]>=0)
                                    {
                                        sub = Valve.portDes-valveFix.fix.portCnt;
                                        prDbg(syspara.typeInfo, "1 <%d>", sub);
                                    }
                                    else
                                    {
                                        sub = Valve.portCur-Negative;
                                        prDbg(syspara.typeInfo, "2 <%d>", sub);
                                    }
                                }
                                else
                                {// ��ԭ��
                                    if(position[AXSV]>=0 && position[AXSV]>=SPR*rdc.rate/2)
                                        position[AXSV] -= SPR*rdc.rate;
                                    sub = Valve.portDes-valveFix.fix.portCnt;
                                    prDbg(syspara.typeInfo, "3 <%d>", sub);
                                }
                                // ����λ���������Ҫ�������źŸ���
                                Valve.limitSignal = Negative*SIGNAL_SUM/valveFix.fix.portCnt;
                                // ��ת��ʱ�򣬵�ǰ�ǰ�ͨ��λ�õ�ʱ����Ҫ��2���ź�ֵ
                                if(Valve.bHalfSeal && !Valve.portCur)
                                {
                                    Valve.limitSignal += SIGNAL_SUM/valveFix.fix.portCnt/2;
                                }
                                coord = getCoord(Negative);
                                AxisMoveRel(AXSV, coord, accel[AXSV], decel[AXSV], speed[AXSV]);
                            }
                        }
                        else
                        {// ָ������Ѱλ
                            if(Valve.portCur==0xff)
                            {
                                Valve.portCur = 0;
                            }
                            if(Valve.dir==CCW)
                            {
                                prDbg(syspara.typeInfo, "CCW=");
                                if(Valve.portDes>Valve.portCur)
                                {// δ��ԭ��
                                    if(position[AXSV]<0)
                                    {
                                        sub = Valve.portDes;
                                        prDbg(syspara.typeInfo, "0 <%d>", sub);
                                    }
                                    else
                                    {
                                        sub = Valve.portDes-valveFix.fix.portCnt;
                                        prDbg(syspara.typeInfo, "1 <%d>", sub);
                                    }
                                    Positive = Valve.portDes-Valve.portCur;
                                }
                                else
                                {
                                    if(position[AXSV]==0)
                                    {
                                        sub = Valve.portDes;
                                        prDbg(syspara.typeInfo, "2 <%d>", sub);
                                    }
                                    else
                                    {// ��ԭ��
                                        sub = Valve.portDes+valveFix.fix.portCnt;
                                        prDbg(syspara.typeInfo, "3 <%d>", sub);
                                    }
                                    Positive = Valve.portDes-Valve.portCur+valveFix.fix.portCnt;
                                }
                                // ����λ���������Ҫ�������źŸ���
                                Valve.limitSignal = Positive*SIGNAL_SUM/valveFix.fix.portCnt;
                                // ����ת���ʱ�򣬵�ǰ�ǰ�ͨ��λ�õ�ʱ����Ҫ�۳�2���ź�ֵ
                                if(Valve.bHalfSeal && !Valve.portCur)
                                {
                                    Valve.limitSignal -= SIGNAL_SUM/valveFix.fix.portCnt/2;
                                }
                                coord = getCoord(Positive);
                                AxisMoveRel(AXSV, -coord, accel[AXSV], decel[AXSV], speed[AXSV]);
                            }
                            else if(Valve.dir==CW)
                            {
                                prDbg(syspara.typeInfo, "CW=");
                                if(Valve.portDes<Valve.portCur)
                                {// δ��ԭ��
                                    if(position[AXSV]<0)
                                    {
                                        sub = Valve.portDes;
                                        prDbg(syspara.typeInfo, "0 <%d>", sub);
                                    }
                                    else
                                    {
                                        sub = Valve.portDes-valveFix.fix.portCnt;
                                        prDbg(syspara.typeInfo, "1 <%d>", sub);
                                    }
                                    Positive = Valve.portCur-Valve.portDes;
                                }
                                else
                                {// ��ԭ��
                                    if(position[AXSV]>=0)// && position[AXSV]>=SPR*rdc.rate/2)
                                        position[AXSV] -= SPR*rdc.rate;
                                    sub = Valve.portDes-valveFix.fix.portCnt;
                                    prDbg(syspara.typeInfo, "2 <%d>", sub);
                                    Positive = Valve.portCur+valveFix.fix.portCnt-Valve.portDes;
                                }
                                // ����λ���������Ҫ�������źŸ���
                                Valve.limitSignal = Positive*SIGNAL_SUM/valveFix.fix.portCnt;
                                // ��ת��ʱ�򣬵�ǰ�ǰ�ͨ��λ�õ�ʱ����Ҫ��2���ź�ֵ
                                if(Valve.bHalfSeal && !Valve.portCur)
                                {
                                    Valve.limitSignal += SIGNAL_SUM/valveFix.fix.portCnt/2;
                                }
                                coord = getCoord(Positive);
                                AxisMoveRel(AXSV, coord, accel[AXSV], decel[AXSV], speed[AXSV]);
                            }
                        }
                        if(Valve.bNewInit)
                            Valve.bNewInit = 0;
        				Valve.status &= ~VALVE_RUN_END; 	// ������н�����־
        				Valve.status |= VALVE_RUNNING; 	    // ��λ���б�־
                        Valve.statusLast = VALVE_RUNNING;
                        syspara.protectTimeOut = 0;
                        syspara.bSkipFirstSig = false;
                    }
                    else if(Valve.portCur==Valve.portDes)
                    {
                        Valve.portDes = 0;
                    }
                    else
                    {
                        getPrePort();
                    }
                }
        	}
            else
            {
                if(Valve.statusLast==VALVE_RUNNING)
                {
                    Valve.statusLast = VALVE_NONE;
                    if(syspara.shiftOnece==true || syspara.reShift==true)
                    {
                        if(Valve.cntSignal==Valve.limitSignal)
                        {
                            prInfo(syspara.typeInfo, "\r\n get signal %d %d", Valve.cntSignal, Valve.limitSignal);
                            Valve.portCur = Valve.portDes;
                            Valve.portDes = 0;
                            Valve.retryTms = 0;
                            Valve.status = VALVE_RUN_END;
                            Valve.cntSignal = 0;
                            Valve.bErr = NONE_ERR;
                            getPrePort();
                        }
                        else
                        {
                            if(Valve.cntSignal)
                            {
                                prInfo(syspara.typeInfo, "\r\n err-0 signal %d %d", Valve.cntSignal, Valve.limitSignal);
                                errProcRun();
                            }
                            else
                            {
                                prInfo(syspara.typeInfo, "\r\n Block Dead");
                            }
                            Valve.cntSignal = 0;
                            Valve.bErr = SIGNAL_CNT_ERR;
                        }
                    }
                    else
                    {
                        syspara.shiftOnece = true;
                        Valve.portCur = Valve.portDes;
                        Valve.portDes = 0;
                        Valve.retryTms = 0;
                        Valve.status = VALVE_RUN_END;
                        Valve.cntSignal = 0;
                        getPrePort();
                    }
                }
            }
        }
    }
}

/*
    ԭ����˿ڹ�����źż�ʱ���ֱ𼤻ͣ���ܣ�ȷ��ֹͣ��λ�ù���ȷ
*/
void ValveLimitDetect(void)
{
    syspara.bInterrupt = true;
}


/*
    PA15�ܽ��ⲿ�жϺ���
*/
void EXTI15_10_IRQHandler(void)
{
    EXTI->PR |= 1<<15;
    if(!VALVE_OPT)
    {// ȱ��
        // �źźϸ��жϣ����ϸ��ȵ��ź�ֵ�ᱻ����
        if(!Valve.bNewInit && sig.bRdPulse==false)
        {
            if(!(Valve.status&VALVE_INITING) && Valve.nowBlockTp)
            {
                if(Valve.nowBlockTp>=(sig.pulseBlock[1]-sig.pulseBlock[3])>>1)
                {
                    ++Valve.cntSignal;
                }
                else
                    return;
            }
        }
    }
    else
    {
        // �źźϸ��жϣ����ϸ��ȵ��ź�ֵ�ᱻ����
        if(!Valve.bNewInit && sig.bRdPulse==false)
        {
            if(!(Valve.status&VALVE_INITING) && Valve.nowBlockTp)
            {
                if(Valve.nowGapTp>=(sig.pulseGap[0]-sig.pulseGap[2])>>1)
                {
                    ++Valve.cntSignal;
                }
                else
                    return;
            }
        }
        // �źŵ�λ��ļ��ٴ����ӽ���λ
        if(sig.bRdPulse==false && syspara.dbgStop==false && !Valve.bNewInit)
        {
            if(Valve.cntSignal==Valve.limitSignal)
            {// ��������
                if(srd[AXSV].dir==CW)
                {
                    srd[AXSV].accel_count = -Valve.fixOrg*rdc.stepP01dgr;
                    if(position[AXSV]<0)
                        position[AXSV] = getCoord(Valve.portDes)+Valve.fixOrg*rdc.stepP01dgr;
                    else
                        position[AXSV] = getCoord(Valve.portDes-valveFix.fix.portCnt)+Valve.fixOrg*rdc.stepP01dgr;

                    srd[AXSV].run_state = DECEL;
                    syspara.dbgStop = true;
                }
                else
                {
                    srd[AXSV].accel_count = (sig.pulseBlock[0]>>1)-sig.pulseBlock[0];
                    if(position[AXSV]<0)
                        position[AXSV] = getCoord(Valve.portDes)-Valve.fixOrg*rdc.stepP01dgr;
                    else
                        position[AXSV] = getCoord(Valve.portDes-valveFix.fix.portCnt)-Valve.fixOrg*rdc.stepP01dgr;

                    srd[AXSV].run_state = DECEL;
                    syspara.dbgStop = true;
                }
            }
        }
    }
    // �����־λ���⿪���򵥴�����ת��ʱ���׸�����źŸ���
    if(syspara.bSkipFirstSig==false)
        syspara.bSkipFirstSig = true;
    if(syspara.pwrOn==true)
        syspara.pwrOn = false;
}

/*
    �ⲿ�жϳ�ʼ��
*/
void EXTI_Init(void)
{
    // �ⲿ�жϽ������������
    GPIOA->CRH &= (GPIO_Crh_P15);
	GPIOA->CRH |= (GPIO_Mode_IN_PU_PD_P15);
    GPIOA->ODR |= (GPIO_Pin_15);
	Ex_NVIC_Config(GPIO_A, 15, RTIR);             // PA15��������������
	Ex_NVIC_Config(GPIO_A, 15, FTIR);             // PA15���������½���
    MY_NVIC_Init(2,0,EXTI15_10_IRQChannel,0);     // ��2��4����ռ��4�����ȼ��������ռ�������
}


/*

*/
void ProcessInterrupt(void)
{
    static uint32 OptGap=0, OptBlock=0;
    static bool printOnce=false;  // ֻ��ӡһ��
    if(syspara.bInterrupt==true)
    {
        syspara.bInterrupt = false;
        if(!VALVE_OPT)
        {// ȱ��
            ++OptGap;
            ++Valve.nowGapTp;
            if(sig.bRdPulse==false)
            {
                if(!printOnce && ((OptGap>(sig.pulseGap[1]+sig.pulseGap[3])<<1 && syspara.bSkipFirstSig==true) || 
                    OptGap>(sig.pulseGap[1]+sig.pulseGap[3])<<2))
                {// ���ź�������ź��쳣����ת�����⣬��ת����������ֵ����(ʹ�����Ƴ˷�)��������ͣ��
                    printOnce = true;
                    Valve.bErr = SIGNAL_CONT_GAP_ERR;
                    errProcRun();
                    prInfo(syspara.typeInfo, "\r\n Continue Gap Err %d", OptGap);
                }
            }

            if(OptBlock)
            {// ��ʱ����Ƭ
                printOnce = false;
                if(syspara.pwrOn==false)
                {
                    if(sig.bRdPulse==true)
                    {
                        prInfo(syspara.typeInfo, "\r\nB%d", OptBlock);
                        sig.pulseBlock[sig.blockNum++] = OptBlock;
                        ++sig.count;
                    }
                    else
                    {
                        if(OptBlock>(sig.pulseBlock[0]-sig.pulseBlock[2]) &&  OptBlock<(sig.pulseBlock[0]+sig.pulseBlock[2]))
                        {
                            if(Valve.passByOne==1&&Valve.initStep==3)
                                Valve.initStep = 0;
                        }
                        else if(OptBlock>(sig.pulseBlock[1]-sig.pulseBlock[3]) && OptBlock<(sig.pulseBlock[1]+sig.pulseBlock[3]))
                        {
                            if(!Valve.initStep)
                            {// 1������������С�ĵ�Ƭ
                                Valve.initStep = 1;
                                //prInfo(syspara.typeInfo, "\r litB");
                            }
                        }
                    }
                }
                OptBlock = 0;
            }
        }
        else
        {// ��Ƭ
            ++OptBlock;
            ++Valve.nowBlockTp;
            if(sig.bRdPulse==false)
            {
                if(!printOnce && ((OptBlock>(sig.pulseBlock[0]+sig.pulseBlock[2])<<1 && syspara.bSkipFirstSig==true) ||
                    OptBlock>(sig.pulseBlock[0]+sig.pulseBlock[2])<<2))
                {// ���ź�������ź��쳣����ת�����⣬��ת����������ֵ����(ʹ�����Ƴ˷�)��������ͣ��
                    printOnce = true;
                    Valve.bErr = SIGNAL_CONT_BLOCK_ERR;
                    errProcRun();
                    prInfo(syspara.typeInfo, "\r\n Continue Block Err %d", OptBlock);
                }
            }
            if(OptGap)
            {// �˴�����ȱ��
                printOnce = false;
                if(syspara.pwrOn==false)
                {
                    if(sig.bRdPulse==true)
                    {
                        prInfo(syspara.typeInfo, "\r\nG%d", OptGap);
                        sig.pulseGap[sig.gapNum++] = OptGap;
                    }
                    else
                    {
                        if(OptGap>(sig.pulseGap[1]-sig.pulseGap[3]) && OptGap<=(sig.pulseGap[1]+sig.pulseGap[3]))
                        {
                            // ÿȦ���������������֤����ת����������������ۻ�
                            Valve.stpCnt = 0;
                            if(srd[AXSV].dir==CW)
                            {// 2�����������ȱ��|�ǳ�ʼ������£�ֻ������ȱ��Ҳ���Լ���ԭ��
                                Valve.initStep = 2;
                                //prInfo(syspara.typeInfo, "\r lrg");
                            }
                        }
                    }
                }
                OptGap = 0;
            }
        }
        if(sig.bRdPulse==false)
        {
            if(Valve.initStep==2 && syspara.pwrOn==false)
            {
                ++Valve.stpCnt;
                if(Valve.stpCnt>=Valve.fixOrg*rdc.stepP01dgr/2)       // �˴�Ϊ���ſ�ԭ��Ĳ���ֵ
                {
                    Valve.stpCnt = 0;
                    Valve.initStep = 3;
                    position[AXSV]= Valve.fixOrg*rdc.stepP01dgr/2;
                    if(Valve.status&VALVE_INITING || Valve.portDes==valveFix.fix.portCnt)
                    {// ��ʼ����ɣ��ҵ�10��λԭ��
                        srd[AXSV].accel_count = -Valve.fixOrg*rdc.stepP01dgr/2;
                        srd[AXSV].run_state = DECEL;
                        position[AXSV] = Valve.fixOrg*rdc.stepP01dgr/2;
                        if(Valve.bHalfSeal)
                        {
                            if(Valve.status&VALVE_INITING)
                            {
                                Valve.status &= ~(VALVE_INITING|VALVE_RUNNING);
                                Valve.portCur = valveFix.fix.portCnt;
                                syspara.shiftOnece = false;
                            }
                            else
                            {
                                if(Valve.cntSignal==Valve.limitSignal)
                                {
                                    Valve.portCur = valveFix.fix.portCnt;
                                    Valve.cntSignal = 0;
                                }
                            }
                        }
                        else
                        {
                            if(Valve.status&VALVE_INITING)
                            {
                                Valve.status &= ~(VALVE_INITING|VALVE_RUNNING);
                                Valve.portCur = valveFix.fix.portCnt;
                                syspara.shiftOnece = false;
                            }
                            else
                            {
                                if(Valve.cntSignal==Valve.limitSignal)
                                {
                                    Valve.portCur = valveFix.fix.portCnt;
                                    Valve.cntSignal = 0;
                                }
                            }
                        }
                        Valve.passByOne = 0;
                        // ���Ŀ��λ��12��λ�����ѵ�Ŀ�꣬��������־
                        if(Valve.portDes==valveFix.fix.portCnt || Valve.bErr)
                        {
                            if(Valve.portDes==valveFix.fix.portCnt)
                                Valve.bErr = NONE_ERR;
                            Valve.status &= ~VALVE_RUNNING;
                            if(!(Valve.status&VALVE_ERR))
                                Valve.status |= VALVE_RUN_END;
                        }
                        // ��ʱ�䣬��֤����������λת��
                        timerPara.timeMilli = 0;
                        if(Valve.bErr)
                            syspara.shiftOnece = true;
                        else
                            Valve.retryTms = 0;
                        getPrePort();
                    }
                }
            }
        }
    }
}



/*
    ���õ�ַΪ64�ŵ�ʱ��ģ����Զ������ջ�����ģʽ
*/
void TestBurn(void)
{
    static uint32 bDir=0;
    if(ModbusPara.mAddrs==ADDR_MAX)
    {
        if(timerPara.timeWaitMill>intCtrl*SEC)
        {// 30����������ģ����ת����һ��ͨ��
            timerPara.timeWaitMill = 0;
            if(Valve.status==VALVE_RUN_END)
            {
                if(Valve.portCur==0xff)
                    Valve.portDes = 1;
                if(!bDir && Valve.portCur==valveFix.fix.portCnt)
                {
                    (!bDir)?(bDir=1):(bDir);
                }
                else if(!bDir && Valve.portCur<valveFix.fix.portCnt)
                {
                    Valve.portDes = Valve.portCur+1;
                    if(Valve.portDes==valveFix.fix.portCnt)
                        bDir = 1;
                }
                else if(bDir && Valve.portCur>1)
                {
                    Valve.portDes = Valve.portCur-1;
                    if(Valve.portDes==1)
                        bDir = 0;
                }
                prInfo(syspara.typeInfo, "\r\n ->%d", Valve.portDes);
            }
        }
    }
}








#define _VALVE_GLOBALS_
#include "common.h"

/*
    ����12λ�ľ������꣬���ź�Ϊ��������һ��ƫ������ԭ�㲹������
    �ﵽ�źŸ����󰴴�ƫ�������м��٣����ض���Ϊ��ǰλ������
    ʹλ�õ���������ȶ��������ź�����׼��������������Ĳ�����ͬ
*/
#define CNT 13
const sint32 posCoord[CNT]=
{

    0,
    -1067 ,
    -2133 ,
    -3200 ,
    -4267 ,
    -5333 ,
    -6400 ,
    -7467 ,
    -8533 ,
    -9600 ,
    -10667 ,
    -11733 ,
    -12800 ,
};

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
        if(Valve.bReInit)
        {
            Valve.bReInit = 0;
            if(Valve.retryTms<RETRY_TIMES)
            {
                printd("\r\n start round");
                VALVE_ENA = ENABLE;
                position[AXSV] = HomePos[AXSV];     // �����ǰ�ļƲ�
                AxisMoveAbs(AXSV, -rdc.stepRound*3, accel[AXSV], decel[AXSV], speed[AXSV]);
                Valve.status |= VALVE_RUNNING;                  // ��λ���б�־
            }
            else
            {
                Valve.ErrBlinkTime = RETRY_TIME_OUT;
                Valve.status = 0;
                VALVE_ENA = DISABLE;
                printd("\r\n Inited retry time out");
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


int getCoord(sint32 destiny)
{
    sint32 dCoord=0;
    dCoord = destiny;
    dCoord *= 360/valveFix.fix.portCnt;
    dCoord *= rdc.stepP1dgr;
    dCoord *= -1; 
    return dCoord;
}


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
    srd[0].run_state = DECEL;
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

                            if(Valve.portDes>Valve.portCur)
                            {
                                Positive = Valve.portDes-Valve.portCur;
                                Negative = Valve.portCur-Valve.portDes+valveFix.fix.portCnt;
                            }
                            else
                            {
                                Positive = Valve.portDes-Valve.portCur+valveFix.fix.portCnt;
                                Negative = Valve.portCur-Valve.portDes;
                            }
                            if(Positive<Negative)
                            {
                                printd("\r\n CCW-");
                                if(position[AXSV]>=0)
                                {
                                    if(Valve.portDes<=Positive)     // ��ԭ��
                                    {
                                        sub = Valve.portDes;
                                        printd("1 <%d>", sub);
                                    }
                                    else
                                    {
                                        sub = Valve.portDes-valveFix.fix.portCnt;
                                        printd("2 <%d>", sub);
                                    }
                                }
                                else
                                {
                                    sub = Valve.portCur+Positive;
                                    printd("3 <%d>", sub);
                                }
                                Valve.limitSignal = Positive*SIGNAL_SUM/valveFix.fix.portCnt;
                            }
                            else if(Positive>=Negative)
                            {
                                printd("\r\n CW-");
                                if(Valve.portCur>Negative)
                                {
                                    if(position[AXSV]>=0)
                                    {
                                        sub = Valve.portDes-valveFix.fix.portCnt;
                                        printd("1 <%d>", sub);
                                    }
                                    else
                                    {
                                        sub = Valve.portCur-Negative;
                                        printd("2 <%d>", sub);
                                    }
                                }
                                else
                                {// ��ԭ��
                                    if(position[AXSV]>=0 && position[AXSV]>=SPR*rdc.rate/2)
                                        position[AXSV] -= SPR*rdc.rate;
                                    sub = Valve.portDes-valveFix.fix.portCnt;
                                    printd("3 <%d>", sub);
                                }
                                Valve.limitSignal = Negative*SIGNAL_SUM/valveFix.fix.portCnt;
                            }
                            coord = getCoord(sub);
                            printd("\r\n go coord:%d->%d", position[AXSV], coord);
                            AxisMoveAbs(AXSV, coord, accel[AXSV], decel[AXSV], speed[AXSV]);
                        }
                        else
                        {// ָ������Ѱλ
                            if(Valve.dir==CCW)
                            {
                                printd("CCW=");
                                if(Valve.portDes>Valve.portCur)
                                {// δ��ԭ��
                                    if(position[AXSV]<0)
                                    {
                                        sub = Valve.portDes;
                                        printd("0 <%d>", sub);
                                    }
                                    else
                                    {
                                        sub = Valve.portDes-valveFix.fix.portCnt;
                                        printd("1 <%d>", sub);
                                    }
                                    Positive = Valve.portDes-Valve.portCur;
                                }
                                else
                                {
                                    if(position[AXSV]==0)
                                    {
                                        sub = Valve.portDes;
                                        printd("2 <%d>", sub);
                                    }
                                    else
                                    {// ��ԭ��
                                        sub = Valve.portDes+valveFix.fix.portCnt;
                                        printd("3 <%d>", sub);
                                    }
                                    Positive = Valve.portDes-Valve.portCur+valveFix.fix.portCnt;
                                }
                                Valve.limitSignal = Positive*SIGNAL_SUM/valveFix.fix.portCnt;
                            }
                            else if(Valve.dir==CW)
                            {
                                printd("CW=");
                                if(Valve.portDes<Valve.portCur)
                                {// δ��ԭ��
                                    if(position[AXSV]<0)
                                    {
                                        sub = Valve.portDes;
                                        printd("0 <%d>", sub);
                                    }
                                    else
                                    {
                                        sub = Valve.portDes-valveFix.fix.portCnt;
                                        printd("1 <%d>", sub);
                                    }
                                    Positive = Valve.portCur-Valve.portDes;
                                }
                                else
                                {// ��ԭ��
                                    if(position[AXSV]>=0)// && position[AXSV]>=SPR*rdc.rate/2)
                                        position[AXSV] -= SPR*rdc.rate;
                                    sub = Valve.portDes-valveFix.fix.portCnt;
                                    printd("2 <%d>", sub);
                                    Positive = Valve.portCur+valveFix.fix.portCnt-Valve.portDes;
                                }
                                Valve.limitSignal = Positive*SIGNAL_SUM/valveFix.fix.portCnt;
                            }
                            coord = getCoord(sub);
                            printd("\r\n go coord:%d->%d", position[AXSV], coord);
                            AxisMoveAbs(AXSV, coord, accel[AXSV], decel[AXSV], speed[AXSV]);
                        }
                        if(Valve.bNewInit)
                            Valve.bNewInit = 0;
                        Valve.OptBlock = 0;
                        Valve.OptGap = 0;
        				Valve.status &= ~VALVE_RUN_END; 	// ������н�����־
        				Valve.status |= VALVE_RUNNING; 	    // ��λ���б�־
                        Valve.statusLast = VALVE_RUNNING;
                        syspara.protectTimeOut = 0;
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
                    if(syspara.shiftOnece==true || syspara.reShift==true)
                    {
                        if(Valve.cntSignal==Valve.limitSignal)
                        {
                            printd("\r\n get signal %d %d", Valve.cntSignal, Valve.limitSignal);
                            Valve.portCur = Valve.portDes;
                            Valve.portDes = 0;
                            Valve.retryTms = 0;
                            Valve.statusLast = 0;
                            Valve.status = VALVE_RUN_END;
                            Valve.cntSignal = 0;
                            getPrePort();
                        }
                        else if(Valve.cntSignal)
                        {
                            printd("\r\n err-0 signal %d %d", Valve.cntSignal, Valve.limitSignal);
                            Valve.status = VALVE_ERR;
                            Valve.cntSignal = 0;
                            Valve.bErr = 1;
                            errProcRun();
                        }
                    }
                    else
                    {
                        syspara.shiftOnece = true;
                        Valve.portCur = Valve.portDes;
                        Valve.portDes = 0;
                        Valve.retryTms = 0;
                        Valve.statusLast = 0;
                        Valve.status = VALVE_RUN_END;
                        Valve.cntSignal = 0;
                        getPrePort();
                    }
                }
            }
        }
        if(sig.bRdPulse==false)
        {
            if(GettCliffSignal()==true)
            {
                if(!Valve.bNewInit)
                    ++Valve.cntSignal;
                printd("\r\n sig %d",Valve.cntSignal);
            }
        }
    }
    else 
    {
        if(Valve.bHalfSeal)
        {
            float tpFloat=0;
            tpFloat = (float)rdc.stepRound/valveFix.fix.portCnt;
            tpFloat /= 2;
            if(!MotionStatus[AXSV])
            {
                AxisMoveAbs(AXSV, -(int)tpFloat, accel[AXSV], decel[AXSV], speed[AXSV]);
                Valve.status &= ~(VALVE_INITING|VALVE_RUNNING);
            }
        }
    }
}

/*
    ԭ����˿ڹ�����źż�ʱ���ֱ𼤻ͣ���ܣ�ȷ��ֹͣ��λ�ù���ȷ
*/
void ValveLimitDetect(void)
{
    if(!VALVE_OPT)
    {// ȱ��
        ++Valve.OptGap;
        if(Valve.OptBlock)
        {// ��ʱ����Ƭ
            if(syspara.pwrOn==false)
            {
                if(sig.bRdPulse==false && Valve.OptBlock<sig.pulseBlock[0]/100) // ���˸���
                    return;
                if(sig.bRdPulse==true)
                {
//                    printd("\r\nL%d",Valve.OptBlock);
                    sig.pulseBlock[sig.num] = Valve.OptBlock;
                    ++sig.count;
                }
                else
                {
                    if(Valve.OptBlock>(sig.pulseBlock[0]-sig.pulseBlock[2]) && Valve.OptBlock<(sig.pulseBlock[0]+sig.pulseBlock[2]))
                    {
                        if(Valve.passByOne==1&&Valve.initStep==3)
                            Valve.initStep = 0;
                    }
                    else if(Valve.OptBlock>(sig.pulseBlock[1]-sig.pulseBlock[3]) && Valve.OptBlock<(sig.pulseBlock[1]+sig.pulseBlock[3]))
                    {
                        if(!Valve.initStep)
                        {// 1������������С�ĵ�Ƭ
                            Valve.initStep = 1;
                            //printd("\r litB");
                        }
                    }
                }
            }
            else
                syspara.pwrOn = false;
            Valve.OptBlock = 0;
        }
    }
    else
    {// ��Ƭ
        ++Valve.OptBlock;
        if(sig.bRdPulse==false)
        {
            if(syspara.shiftOnece==true && syspara.dbgStop==false)
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
                        if((sig.pulseBlock[0]-Valve.OptBlock)<2*Valve.fixOrg*rdc.stepP01dgr)
                        {
                            srd[AXSV].accel_count = -Valve.fixOrg*rdc.stepP01dgr;
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
        }
        
        if(Valve.OptGap)
        {// �˴�����ȱ��
            if(syspara.pwrOn==false)
            {
                if(sig.bRdPulse==false && Valve.OptGap<sig.pulseGap[0]/100) // ���˸���
                    return;
                if(sig.bRdPulse==true)
                {
//                    printd("\r\nH%d",Valve.OptGap);
                    sig.pulseGap[sig.num++] = Valve.OptGap;
                }
                else
                {
                    if(Valve.OptGap>(sig.pulseGap[1]-sig.pulseGap[3]) && Valve.OptGap<=(sig.pulseGap[1]+sig.pulseGap[3]))
                    {
                        // ÿȦ���������������֤����ת����������������ۻ�
                        Valve.stpCnt = 0;
                        if(srd[AXSV].dir==CW)
                        {// 2�����������ȱ��|�ǳ�ʼ������£�ֻ������ȱ��Ҳ���Լ���ԭ��
                            Valve.initStep = 2;
                            //printd("\r lrg");
                        }
                    }
                }
            }
            else
                syspara.pwrOn = false;
            Valve.OptGap = 0;
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
                Valve.bGetOrg = 1;
                if(Valve.status&VALVE_INITING || Valve.portDes==valveFix.fix.portCnt)
                {// ��ʼ����ɣ��ҵ�10��λԭ��
                    srd[AXSV].accel_count = -Valve.fixOrg*rdc.stepP01dgr/2;
                    srd[AXSV].run_state = DECEL;
                    position[AXSV] = Valve.fixOrg*rdc.stepP01dgr/2;
                    if(Valve.bHalfSeal)
                    {
                        if(Valve.status&VALVE_INITING)
                        {
                            Valve.portCur = 0xff;
                            syspara.shiftOnece = false;
                        }
                        else
                        {
                            if(Valve.cntSignal==Valve.limitSignal)
                            {
                                Valve.portCur = valveFix.fix.portCnt;
                                Valve.cntSignal = 0;
                            }
                            else if(Valve.cntSignal)
                            {
                                printd("\r\n err-1 signal %d %d", Valve.cntSignal, Valve.limitSignal);
                                Valve.status = VALVE_ERR;
                                Valve.cntSignal = 0;
                                Valve.bErr = 1;
                                errProcRun();
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
                            else if(Valve.cntSignal)
                            {
                                printd("\r\n err-2 signal %d %d", Valve.cntSignal, Valve.limitSignal);
                                Valve.status = VALVE_ERR;
                                Valve.cntSignal = 0;
                                Valve.bErr = 1;
                                errProcRun();
                            }
                        }
                    }
                    
                    if(!Valve.bErr)
                    {
                        Valve.retryTms = 0;
                        Valve.portDes = 0;
                    }
                    else
                    {
                        Valve.bErr = 0;
                    }
                    Valve.passByOne = 0;
                    Valve.status &= ~VALVE_RUNNING;
                    if(!(Valve.status&VALVE_ERR))
                        Valve.status |= VALVE_RUN_END;
                    // ��ʱ�䣬��֤����������λת��
                    timerPara.timeMilli = 0;
                    getPrePort();
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
                printd("\r\n ->%d", Valve.portDes);
            }
        }
    }
}








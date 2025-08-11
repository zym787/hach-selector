#define _VALVE_GLOBALS_
#include "common.h"

/*
    定好12位的绝对坐标，以信号为辅，设置一个偏移量（原点补偿），
    达到信号个数后按此偏移量进行减速，并重定义为当前位的坐标
    使位置的坐标相对稳定，如遇信号量不准，即报错，正反向的补偿不同
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
                prInfo(syspara.typeInfo, "\r\n 启动电机");
                VALVE_ENA = ENABLE;
                position[AXSV] = HomePos[AXSV];     // 清除当前的计步
                AxisMoveAbs(AXSV, -rdc.stepRound*3, accel[AXSV], decel[AXSV], speed[AXSV]);
                Valve.status |= VALVE_RUNNING;                  // 置位运行标志
            }
            else
            {
                Valve.ErrBlinkTime = RETRY_TIME_OUT;
                Valve.status = VALVE_ERR;
                VALVE_ENA = DISABLE;
                prInfo(syspara.typeInfo, "\r\n 重复位失败");
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
    按孔位距离计算需要走多少步数
    由于是按信号数定位，需要多走，此处多走半个通道
    当信号数计数到了之后，在中断处按补偿值进行减速动作，保证每次到达的目标位置都在信号点附近
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
    按补偿值计算减速步数
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
    // 进入减速段
    srd[AXSV].run_state = DECEL;
}

/*
    初始化完成后进行通道寻位，预给出两圈的行程，肯定会找到两次目标位置
    找到一个通道点，位置加1，直到找到目标位置，激活急停
    如果超过一圈没有找到目标位置，启动重新初始化，重新初始化的次数超过3次，报错退出
*/
void ProcessValve(void)
{
    int Positive=0, Negative=0, sub=0, coord=0;     // 正向与反向差值
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
                        {// 就近原则找位
                            // 找出最近方向
                            if(Valve.portCur==0xff)
                            {
                                Valve.portCur = 0;
                            }

                            // 计算前后两个方向当前位置与目标位置距离多少个孔位
                            // 就近原则下取小值的那个方向
                            if(Valve.portDes>Valve.portCur)
                            {// 目标位置在当前位置后面
                                Positive = Valve.portDes-Valve.portCur;
                                Negative = Valve.portCur-Valve.portDes+valveFix.fix.portCnt;
                            }
                            else
                            {// 目标位置在当前位置前面
                                Positive = Valve.portDes-Valve.portCur+valveFix.fix.portCnt;
                                Negative = Valve.portCur-Valve.portDes;
                            }
                            if(Positive<Negative)
                            {
                                prDbg(syspara.typeInfo, "\r\n CCW-");
                                if(position[AXSV]>=0)
                                {
                                    if(Valve.portDes<=Positive)     // 过原点
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
                                // 按孔位距离计算需要产生的信号个数
                                Valve.limitSignal = Positive*SIGNAL_SUM/valveFix.fix.portCnt;
                                // 常规转向的时候，当前是半通道位置的时候需要扣除2个信号值
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
                                {// 过原点
                                    if(position[AXSV]>=0 && position[AXSV]>=SPR*rdc.rate/2)
                                        position[AXSV] -= SPR*rdc.rate;
                                    sub = Valve.portDes-valveFix.fix.portCnt;
                                    prDbg(syspara.typeInfo, "3 <%d>", sub);
                                }
                                // 按孔位距离计算需要产生的信号个数
                                Valve.limitSignal = Negative*SIGNAL_SUM/valveFix.fix.portCnt;
                                // 反转的时候，当前是半通道位置的时候需要加2个信号值
                                if(Valve.bHalfSeal && !Valve.portCur)
                                {
                                    Valve.limitSignal += SIGNAL_SUM/valveFix.fix.portCnt/2;
                                }
                                coord = getCoord(Negative);
                                AxisMoveRel(AXSV, coord, accel[AXSV], decel[AXSV], speed[AXSV]);
                            }
                        }
                        else
                        {// 指定方向寻位
                            if(Valve.portCur==0xff)
                            {
                                Valve.portCur = 0;
                            }
                            if(Valve.dir==CCW)
                            {
                                prDbg(syspara.typeInfo, "CCW=");
                                if(Valve.portDes>Valve.portCur)
                                {// 未过原点
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
                                    {// 过原点
                                        sub = Valve.portDes+valveFix.fix.portCnt;
                                        prDbg(syspara.typeInfo, "3 <%d>", sub);
                                    }
                                    Positive = Valve.portDes-Valve.portCur+valveFix.fix.portCnt;
                                }
                                // 按孔位距离计算需要产生的信号个数
                                Valve.limitSignal = Positive*SIGNAL_SUM/valveFix.fix.portCnt;
                                // 常规转向的时候，当前是半通道位置的时候需要扣除2个信号值
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
                                {// 未过原点
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
                                {// 过原点
                                    if(position[AXSV]>=0)// && position[AXSV]>=SPR*rdc.rate/2)
                                        position[AXSV] -= SPR*rdc.rate;
                                    sub = Valve.portDes-valveFix.fix.portCnt;
                                    prDbg(syspara.typeInfo, "2 <%d>", sub);
                                    Positive = Valve.portCur+valveFix.fix.portCnt-Valve.portDes;
                                }
                                // 按孔位距离计算需要产生的信号个数
                                Valve.limitSignal = Positive*SIGNAL_SUM/valveFix.fix.portCnt;
                                // 反转的时候，当前是半通道位置的时候需要加2个信号值
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
        				Valve.status &= ~VALVE_RUN_END; 	// 清除运行结束标志
        				Valve.status |= VALVE_RUNNING; 	    // 置位运行标志
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
    原点与端口光耦的信号激时，分别激活急停功能，确保停止的位置够精确
*/
void ValveLimitDetect(void)
{
    syspara.bInterrupt = true;
}


/*
    PA15管脚外部中断函数
*/
void EXTI15_10_IRQHandler(void)
{
    EXTI->PR |= 1<<15;
    if(!VALVE_OPT)
    {// 缺口
        // 信号合格判断，不合格宽度的信号值会被丢弃
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
        // 信号合格判断，不合格宽度的信号值会被丢弃
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
        // 信号到位后的减速处理，接近孔位
        if(sig.bRdPulse==false && syspara.dbgStop==false && !Valve.bNewInit)
        {
            if(Valve.cntSignal==Valve.limitSignal)
            {// 修正坐标
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
    // 处理标志位避免开机或单次运行转向时的首个宽度信号干扰
    if(syspara.bSkipFirstSig==false)
        syspara.bSkipFirstSig = true;
    if(syspara.pwrOn==true)
        syspara.pwrOn = false;
}

/*
    外部中断初始化
*/
void EXTI_Init(void)
{
    // 外部中断脚输入输出定义
    GPIOA->CRH &= (GPIO_Crh_P15);
	GPIOA->CRH |= (GPIO_Mode_IN_PU_PD_P15);
    GPIOA->ODR |= (GPIO_Pin_15);
	Ex_NVIC_Config(GPIO_A, 15, RTIR);             // PA15触发类型上升沿
	Ex_NVIC_Config(GPIO_A, 15, FTIR);             // PA15触发类型下降沿
    MY_NVIC_Init(2,0,EXTI15_10_IRQChannel,0);     // 组2，4组抢占级4组优先级，最高抢占最高优先
}


/*

*/
void ProcessInterrupt(void)
{
    static uint32 OptGap=0, OptBlock=0;
    static bool printOnce=false;  // 只打印一次
    if(syspara.bInterrupt==true)
    {
        syspara.bInterrupt = false;
        if(!VALVE_OPT)
        {// 缺口
            ++OptGap;
            ++Valve.nowGapTp;
            if(sig.bRdPulse==false)
            {
                if(!printOnce && ((OptGap>(sig.pulseGap[1]+sig.pulseGap[3])<<1 && syspara.bSkipFirstSig==true) || 
                    OptGap>(sig.pulseGap[1]+sig.pulseGap[3])<<2))
                {// 无信号跳变的信号异常，堵转报错监测，堵转超出误差最大值两倍(使用左移乘法)报错，立即停机
                    printOnce = true;
                    Valve.bErr = SIGNAL_CONT_GAP_ERR;
                    errProcRun();
                    prInfo(syspara.typeInfo, "\r\n Continue Gap Err %d", OptGap);
                }
            }

            if(OptBlock)
            {// 此时处理挡片
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
                            {// 1步，首先碰到小的挡片
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
        {// 挡片
            ++OptBlock;
            ++Valve.nowBlockTp;
            if(sig.bRdPulse==false)
            {
                if(!printOnce && ((OptBlock>(sig.pulseBlock[0]+sig.pulseBlock[2])<<1 && syspara.bSkipFirstSig==true) ||
                    OptBlock>(sig.pulseBlock[0]+sig.pulseBlock[2])<<2))
                {// 无信号跳变的信号异常，堵转报错监测，堵转超出误差最大值两倍(使用左移乘法)报错，立即停机
                    printOnce = true;
                    Valve.bErr = SIGNAL_CONT_BLOCK_ERR;
                    errProcRun();
                    prInfo(syspara.typeInfo, "\r\n Continue Block Err %d", OptBlock);
                }
            }
            if(OptGap)
            {// 此处处理缺口
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
                            // 每圈进行数据清除，保证长期转动下来不会有误差累积
                            Valve.stpCnt = 0;
                            if(srd[AXSV].dir==CW)
                            {// 2步，碰到大的缺口|非初始化情况下，只碰到大缺口也可以激活原点
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
                if(Valve.stpCnt>=Valve.fixOrg*rdc.stepP01dgr/2)       // 此处为１号口原点的补偿值
                {
                    Valve.stpCnt = 0;
                    Valve.initStep = 3;
                    position[AXSV]= Valve.fixOrg*rdc.stepP01dgr/2;
                    if(Valve.status&VALVE_INITING || Valve.portDes==valveFix.fix.portCnt)
                    {// 初始化完成，找到10号位原点
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
                        // 如果目标位是12号位，即已到目标，清除错误标志
                        if(Valve.portDes==valveFix.fix.portCnt || Valve.bErr)
                        {
                            if(Valve.portDes==valveFix.fix.portCnt)
                                Valve.bErr = NONE_ERR;
                            Valve.status &= ~VALVE_RUNNING;
                            if(!(Valve.status&VALVE_ERR))
                                Valve.status |= VALVE_RUN_END;
                        }
                        // 清时间，保证不会连续复位转动
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
    设置地址为64号的时候，模块会自动启动烧机测试模式
*/
void TestBurn(void)
{
    static uint32 bDir=0;
    if(ModbusPara.mAddrs==ADDR_MAX)
    {
        if(timerPara.timeWaitMill>intCtrl*SEC)
        {// 30秒间隔，启动模块运转到下一个通道
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








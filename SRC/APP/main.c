#define _MAIN_H_GLOBALS_
#include "common.h"

uint8 moduleAddrDflt = 0, valveFixDflt = 0, valveFixDir=0, valvePortCnt=12, IntDflt=5, SpdDflt=3, protocalDflt=MY_MODBUS;
uint8 bRdpDflt = 0;

void ParameterInit(void)
{
    float ftemp=0;
    uint8 ReadBuf[LEN_SYMBOL]={0,0,0,0,0,0,0,0};

    prInfo(PR_INFO, "\r\n 版本号:%s", SOFT_VS);
    I2CPageRead_Nbytes(ADDR_BOARD_ID, LEN_BOARD_ID, ReadBuf);
    if(ReadBuf[0]==0x88 && ReadBuf[1]==0x66)
    {
        prInfo(PR_INFO, "\r Read stored data");

        I2CPageRead_Nbytes(ADDR_MODULE_NUM, LEN_MODULE_NUM, &ModbusPara.mAddrs);
        prInfo(PR_INFO, "\r Addr:%d", ModbusPara.mAddrs);

        I2CPageRead_Nbytes(ADDR_PORT_CNT, LEN_PORT_CNT, &valveFix.fix.portCnt);
        (valveFix.fix.portCnt&&valveFix.fix.portCnt>32)?(valveFix.fix.portCnt=10):(valveFix.fix.portCnt);
        ftemp = (float)SIGNAL_SUM/valveFix.fix.portCnt;
        ftemp += 0.5;
        Valve.threshSignal = ftemp;
        prInfo(PR_INFO, "\r Port Cnt:%d %d", valveFix.fix.portCnt, Valve.threshSignal);

        I2CPageRead_Nbytes(ADDR_VALVE_FIX, LEN_VALVE_FIX, &Valve.fixOrg);
        prInfo(PR_INFO, "\r Fix:%d", Valve.fixOrg);
        if(Valve.fixOrg<=5)
        {
            Valve.fixOrg = 5;
            prInfo(PR_INFO, "\r 补偿至少0.%d度", Valve.fixOrg);
        }
        else if(Valve.fixOrg>40)
        {
            Valve.fixOrg = 40;
            prInfo(PR_INFO, "\r 补偿至多%d度", Valve.fixOrg/10);
        }

        I2CPageRead_Nbytes(ADDR_DIR_FIX, LEN_DIR_FIX, &valveFix.fix.dirGap);
        prInfo(PR_INFO, "\r Dir:%d", valveFix.fix.dirGap);

        prInfo(syspara.typeInfo, "\r Fix:");
        for(uint32 i=0; i<valveFix.fix.portCnt; i++)
            prInfo(PR_INFO, " %d", valveFix.array[i]);

        I2CPageRead_Nbytes(ADDR_INTVL, LEN_INTVL, &intCtrl);
        prInfo(PR_INFO, "\r Interval:%d", intCtrl);

        I2CPageRead_Nbytes(ADDR_SPD, LEN_SPD, &Valve.spd);
        if(!Valve.spd || Valve.spd>SPD_LMT)
            Valve.spd = SPD_VALVE;
        prInfo(PR_INFO, "\r Spd:%d", Valve.spd);

        I2CPageRead_Nbytes(ADDR_RDC_RATE, LEN_RDC_RATE, &rdc.rate);
		switch(rdc.rate)
		{
			case RDC01:
				rdc.stepP1dgr = STEPS_1_DEGREE_RD01;
				rdc.stepP01dgr = STEPS_01_DEGREE_RD01;
				break;
            case RDC04:
                rdc.stepP1dgr = STEPS_1_DEGREE_RD04;
                rdc.stepP01dgr = STEPS_01_DEGREE_RD04;
                break;
			case RDC10:
				rdc.stepP1dgr = STEPS_1_DEGREE_RD10;
				rdc.stepP01dgr = STEPS_01_DEGREE_RD10;
				break;
			case RDC16:
				rdc.stepP1dgr = STEPS_1_DEGREE_RD16;
				rdc.stepP01dgr = STEPS_01_DEGREE_RD16;
				break;
		}
		rdc.stepRound = P_ROUND;
		rdc.stepRound *= SCALE;
		rdc.stepRound *= rdc.rate;
        prInfo(PR_INFO, "\r\n rate %d round %d", rdc.rate, rdc.stepRound);
        
        I2CPageRead_Nbytes(ADDR_SPD, LEN_SPD, &Valve.spd);
        if(!Valve.spd || Valve.spd>SPD_LMT)
            Valve.spd = SPD_VALVE;
        
        speed[AXSV] = accel[AXSV] = decel[AXSV] = 100;
        speed[AXSV] *= (Valve.spd);
        speed[AXSV] *= (rdc.rate);
        accel[AXSV] *= (Valve.spd);
        accel[AXSV] *= (rdc.rate);
        decel[AXSV] *= (Valve.spd);
        decel[AXSV] *= (rdc.rate);
        
        I2CPageRead_Nbytes(ADDR_SN, LEN_SN, Valve.SnCode);
        I2CPageRead_Nbytes(ADDR_PROTOCAL, LEN_PROTOCAL, &syspara.typeProtocal);

        I2CPageRead_Nbytes(ADDR_RDP, LEN_RDP, &syspara.bRdPulse);
        if(syspara.bRdPulse==true)
            prInfo(PR_INFO, "\r\n enable pulse read");
        else
            prInfo(PR_INFO, "\r\n disable pulse read");

        I2CPageRead_Nbytes(ADDR_SYMBOL, LEN_SYMBOL, ReadBuf);
        // 重新排序，B0为常规齿，B1为小齿，B2为常规齿误差值，B3为小齿误差值，G0为常规口，G1为大口，G2为常规口误差，G3为大口误差
        sig.pulseBlock[0] = ReadBuf[0];
        sig.pulseBlock[0] <<= 8;
        sig.pulseBlock[0] |= ReadBuf[1];
        uint32 temp=0;
        temp = sig.pulseBlock[0]*PERCENT_TOLL;
        sig.pulseBlock[2] = temp/PERCENT;
        
        sig.pulseBlock[1] = ReadBuf[2];
        sig.pulseBlock[1] <<= 8;
        sig.pulseBlock[1] |= ReadBuf[3];
        temp = sig.pulseBlock[1]*PERCENT_TOLL;
        sig.pulseBlock[3] = temp/PERCENT;
        
        sig.pulseGap[0] = ReadBuf[4];
        sig.pulseGap[0] <<= 8;
        sig.pulseGap[0] |= ReadBuf[5];
        temp = sig.pulseGap[0]*PERCENT_TOLL;
        sig.pulseGap[2] = temp/PERCENT;
        
        sig.pulseGap[1] = ReadBuf[6];
        sig.pulseGap[1] <<= 8;
        sig.pulseGap[1] |= ReadBuf[7];
        temp = sig.pulseGap[1]*PERCENT_TOLL;
        sig.pulseGap[3] = temp/PERCENT;
        prInfo(PR_INFO, "\r\n 常规齿 %d, 小齿%d, TOLL %d, %d",
        sig.pulseBlock[0], sig.pulseBlock[1], sig.pulseBlock[2], sig.pulseBlock[3]);
        prInfo(PR_INFO, "\r\n 常规口%d, 大口%d, TOLL %d, %d",
        sig.pulseGap[0], sig.pulseGap[1], sig.pulseGap[2], sig.pulseGap[3]);
        I2CPageRead_Nbytes(ADDR_HALF_SEAL, LEN_HALF_SEAL, &Valve.bHalfSeal);
        prInfo(PR_INFO, "\r\n 半通标志%d", Valve.bHalfSeal);
    }
    else
    {
        prInfo(PR_INFO, "\r Write default data");
        ReadBuf[0] = 0x88;
        ReadBuf[1] = 0x66;
        I2CPageWrite_Nbytes(ADDR_BOARD_ID, LEN_BOARD_ID, ReadBuf);
        I2CPageWrite_Nbytes(ADDR_MODULE_NUM, LEN_MODULE_NUM, &moduleAddrDflt);
        ModbusPara.mAddrs = moduleAddrDflt;
        I2CPageWrite_Nbytes(ADDR_PORT_CNT, LEN_PORT_CNT, &valvePortCnt);
        valveFix.fix.portCnt = valvePortCnt;
        ftemp = (float)SIGNAL_SUM/valveFix.fix.portCnt;
        ftemp += 0.5;
        Valve.threshSignal = ftemp-1;
        prInfo(PR_INFO, "\r Port Cnt:%d %d", valveFix.fix.portCnt, Valve.threshSignal);

        I2CPageWrite_Nbytes(ADDR_VALVE_FIX, LEN_VALVE_FIX, &valveFixDflt);
        Valve.fixOrg = valveFixDflt;
        I2CPageWrite_Nbytes(ADDR_DIR_FIX, LEN_DIR_FIX, &valveFixDir);
        valveFix.fix.dirGap = valveFixDir;
        I2CPageWrite_Nbytes(ADDR_INTVL, LEN_INTVL, &IntDflt);
        intCtrl = IntDflt;
        
        rdc.rate = 10;
        I2CPageWrite_Nbytes(ADDR_RDC_RATE, LEN_RDC_RATE, &rdc.rate);
		rdc.stepP1dgr = STEPS_1_DEGREE_RD10;
		rdc.stepP01dgr = STEPS_01_DEGREE_RD10;
		rdc.stepRound = P_ROUND;
		rdc.stepRound *= SCALE;
		rdc.stepRound *= rdc.rate;
        prInfo(PR_INFO, "\r\n rate %d round %d", rdc.rate, rdc.stepRound);
        I2CPageWrite_Nbytes(ADDR_SPD, LEN_SPD, &SpdDflt);
        Valve.spd = SpdDflt;
        speed[AXSV] = accel[AXSV] = decel[AXSV] = 100;
        speed[AXSV] *= (Valve.spd);
        speed[AXSV] *= (rdc.rate);
        accel[AXSV] *= (Valve.spd);
        accel[AXSV] *= (rdc.rate);
        decel[AXSV] *= (Valve.spd);
        decel[AXSV] *= (rdc.rate);
        
        memset(Valve.SnCode, 0, sizeof(Valve.SnCode));
        I2CPageWrite_Nbytes(ADDR_SN, LEN_SN, Valve.SnCode);
        I2CPageWrite_Nbytes(ADDR_PROTOCAL, LEN_PROTOCAL, &protocalDflt);

        syspara.bRdPulse = (bool)bRdpDflt;
        if(syspara.bRdPulse==true)
            prInfo(PR_INFO, "\r\n enable pulse read");
        else
            prInfo(PR_INFO, "\r\n disable pulse read");
    }
    getOptStartStatus();
    VALVE_ENA = ON;
    Valve.status = VALVE_INITING;
    Valve.ErrBlinkTime = NORMAL_BLINK;
    Valve.passByOne = 0;
    Valve.bNewInit = 1;
    Valve.retryTms = 0;
    syspara.protectTimeOut = 0;
    syspara.pwrOn = true;
    syspara.reShift = false;
//    #ifdef INFO_DEBUG
//    syspara.typeInfo = PR_NONE;
//    #else
//    syspara.typeInfo = PR_ALL;
//    #endif
    syspara.typeInfo = PR_INFO;
    syspara.comInfo = PR_INFO;
}
 

/*
    
*/
void GPIOInit(void)
{
    RCC->APB2ENR |= (RCC_APB2Periph_GPIOB);
    GPIOB->CRL &= (GPIO_Crl_P1);
    GPIOB->CRL |= (GPIO_Mode_Out_PP_50MHz_P1);

}


/*
    PA15管脚外部中断函数
*/
void EXTI15_10_IRQHandler(void)
{
    if(sig.bRdPulse==false)
    {
        if(!Valve.bNewInit)
        {
            syspara.bEXTI = true;
        }
    }
    EXTI->PR |= 1<<15;
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


void errProcRun(void)
{
    if(++Valve.retryTms<=RETRY_TIMES)
    {
        Valve.status = VALVE_INITING;
        Valve.bNewInit = 1;
        syspara.pwrOn = true;
        syspara.reShift = true;
        Valve.initStep = 0;
        syspara.protectTimeOut = 0;
        Valve.cntSignal = 0;
//        Valve.status = VALVE_INITING;
//        Valve.bNewInit = 1;
//        syspara.pwrOn = true;
//        Valve.initStep = 0;
//        syspara.protectTimeOut = 0;
//        Valve.stpCnt = 0;
//        Valve.bErr = 0;
        prInfo(syspara.typeInfo, "\r\n reShift %d", Valve.retryTms);
    }
    else
    {
        Valve.portDes = 0;
        Valve.ErrBlinkTime = RETRY_TIME_OUT;
        Valve.status = VALVE_ERR;
        VALVE_ENA = DISABLE;
        prInfo(syspara.typeInfo, "\r\n reShift times out");
    }
}


void every50MilliSecDoing(void)
{
    if(timerPara.everySec>50)
    {
        timerPara.everySec = 0;

        if((Valve.status==VALVE_RUNNING&&syspara.protectTimeOut>5*SEC)
            ||(Valve.status&VALVE_INITING&&syspara.protectTimeOut>10*SEC))
        {// 单通道间做5秒的超时处理，避免长时间堵转烧坏电路
            if(!(Valve.status&VALVE_ERR))
            {
                Valve.portDes = 0;
                Valve.ErrBlinkTime = RETRY_TIME_OUT;
                Valve.status = VALVE_ERR;
                VALVE_ENA = DISABLE;
                prInfo(syspara.typeInfo, "\r\n timeout");
            }
        }

        if(!Valve.bHalfSeal)
        {
            if(Valve.status==VALVE_RUN_END && !MotionStatus[AXSV] && !Valve.bErr && Valve.bNewInit)
            {
                Valve.portDes = 1;
                Valve.dir = 0xff;
                syspara.shiftOnece = true;
            }
        }
        else
        {
            if(Valve.status==VALVE_RUN_END && !MotionStatus[AXSV] && !Valve.bErr && Valve.bNewInit)
            {
                if(Valve.portCur!=0xff)
                {
                    float tpFloat=0;
                    tpFloat = (float)rdc.stepRound/valveFix.fix.portCnt;
                    tpFloat /= 2;
                    if(!MotionStatus[AXSV])
                    {
                        AxisMoveAbs(AXSV, -(int)tpFloat, accel[AXSV], decel[AXSV], speed[AXSV]);
                        Valve.status &= ~(VALVE_INITING|VALVE_RUNNING);
                    }
                    Valve.portCur = 0xff;
                    syspara.shiftOnece = true;
                    prInfo(syspara.typeInfo, "\r\n go half");
                }
            }
        }
    }
}


/*

*/
int main(void)
{
	Stm32_Clock_Init(9);	        //系统时钟设置
	delay_init(72);	   	 	        //延时初始化
    JTAG_Set(JTAG_SWD_DISABLE);

	Usart1_Init(72, 115200);	 	//串口初始化为115200
    iic_INIT();
    ConfigValve();
 	TIM2_Init(999,71);              //10Khz的计数频率
    TIM4_Init(65535,35);            //X轴脉冲定时器
    GPIOInit();
    EXTI_Init();
    delay_ms(100);
    BootInterface();
    ParameterInit();
    if(syspara.typeProtocal==MY_MODBUS)
        ModbusInit();
    else
        CommInit();

	while(1)
	{
        if(syspara.typeProtocal==MY_MODBUS)
            ModbusProces();
        else
            UsartProcess();
        InitValve();
        ProcessInterrupt();
        if(Valve.status!=VALVE_ERR)
        {
            ProcessValve();
        }
        every50MilliSecDoing();
        SignalScan();
        TestBurn();
        InfoErrPrint();
        DebugOut();
    }
}

void DebugOut(void)
{
    if(!MotionStatus[AXSV] && syspara.dbgStop==true)
    {
        syspara.dbgStop = false;
        prInfo(syspara.typeInfo, "\r\n get point");
    }
    if(timerPara.timeDbg>SEC)
    {
        timerPara.timeDbg = 0;
        LED_WORK = !LED_WORK;
        if(Valve.bGetOrg)
        {
            Valve.bGetOrg = 0;
            prInfo(syspara.typeInfo, "\r\n get/pass org %d", position[AXSV]);
        }
        prDbg(syspara.typeInfo, "\r\n >>status:0x%02x,port:0x%02x,dest:0x%02x,opt:%d [%d %d]", 
        Valve.status, Valve.portCur, Valve.portDes, VALVE_OPT, MotionStatus[AXSV], position[AXSV]);
//        prDbg(syspara.typeInfo, "\r\n >> %d %d %d %d", syspara.shiftOnece, Valve.bNewInit, syspara.reShift, Valve.bErr);
    }
}






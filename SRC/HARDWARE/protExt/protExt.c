#define _PROTEXT_GLOBALS_
#include "common.h"

void CommInit(void)
{
    protext.stepCnt = PROTOCOL_HEAD;
    protext.rxCount = 0;
    protext.rxTimeOn = 0;
    protext.rxTimeCnt = 0;
	I2CPageRead_Nbytes(ADDR_BAUD, LEN_BAUD, &syspara.bdrate);
    if(syspara.bdrate==2)
    {
        Usart2_Init(36, BAUD_RATE_19200);				//串口初始化默认为19200
    	delay_ms(100);
    	Usart3_Init(36, BAUD_RATE_19200);	            // 串口2 485初始化为19200
    	delay_ms(100);
        prInfo(PR_INFO, "\r\n baud 19200");
    }
    else
    {
        Usart2_Init(36, BAUD_RATE_9600);				//串口初始化默认为9600
    	delay_ms(100);
    	Usart3_Init(36, BAUD_RATE_9600);	        // 串口2 485初始化为9600
    	delay_ms(100);
        prInfo(PR_INFO, "\r\n baud 9600");
    }
	delay_ms(100);
}


/*
*/
void RxDataProcessEnd(void)
{
    protext.stepCnt = PROTOCOL_HEAD;
    protext.rxCount = 0;
    protext.rxTimeOn = 0;
    protext.rxTimeCnt = 0;
}

/*
*/
void ERR_Reset(void)
{
	protext.rxCount = 0;
	protext.stepCnt = PROTOCOL_HEAD;
	protext.f_RxErr = 2;
    memset(protext.usartBuf, 0, sizeof(protext.usartBuf));
}


/*
*/
void RCV_Buf(unsigned char buf)
{
	protext.usartBuf[protext.rxCount++] = buf;
    protext.time = 0;
}


void RxUsart(void)
{
	switch(protext.stepCnt)
	{
		case PROTOCOL_HEAD:
			if(*(protext.usartBuf+0) == HEAD_BYTE)
			{
				protext.stepCnt = PROTOCOL_ADDR;
			}
            else
            {
			    ERR_Reset();
            }
			break;
		case PROTOCOL_ADDR:
			if(*(protext.usartBuf+1) == ModbusPara.mAddrs)
			{
				protext.stepCnt = PROTOCOL_COMMAND;
			}
            else
            {
			    ERR_Reset();
            }
			break;
		case PROTOCOL_COMMAND:
			protext.stepCnt = PROTOCOL_OK;
			break;
		default:
			break;
	}
}


/*

*/
uint8 CommCheckSum(uint32 lenth, uint8 *sendbuf)
{
	uint16 cnt;
    uint32 checkSum=0;

    checkSum = 0;
	if(lenth)
	{
		for(cnt=0; cnt<lenth; cnt++)
		{
			checkSum += *(sendbuf+cnt);
		}
	}
    if(cnt==lenth)
        return (uint8)checkSum;
    else
        return 0;
}


/*
*/
void AskStaProcess(uint32 sta8)
{
    uint32 sdLen=0, checkSum=0;

    memset(protext.replyBuf, 0, sizeof(protext.replyBuf));
    protext.replyBuf[0] = HEAD_BYTE;
    protext.replyBuf[1] = P_ADDR;

    switch(P_COMMAND)
    {
        case SHIFT_NML:
        case SHIFT_DE:
        case SHIFT_IN:
            if(!MotionStatus[AXSV] && Valve.status==VALVE_RUN_END && !Valve.bErrRetn)
            {
                protext.replyBuf[2] = 0x00;
                protext.replyBuf[3] = 0x00;
                protext.replyBuf[4] = 0x00;
                protext.replyBuf[5] = 0x00;
            }
            else
            {
                #ifndef INFO_DEBUG
                Valve.bErrRetn = 0;
                #endif
                protext.replyBuf[2] = 0x00;
                protext.replyBuf[3] = 0x00;
                protext.replyBuf[4] = 0x00;
                protext.replyBuf[5] = 0x01;
            }
            break;
        case ACTN_RESET:
            protext.replyBuf[2] = 0x00;
            protext.replyBuf[3] = 0x00;
            protext.replyBuf[4] = 0x00;
            protext.replyBuf[5] = 0x00;
            break;
        case WR_CUST_DATA:
            protext.replyBuf[2] = 0x00;
            protext.replyBuf[3] = 0x00;
            protext.replyBuf[4] = 0x00;
            protext.replyBuf[5] = 0x00;
            break;
        case RD_CUST_DATA:
            protext.replyBuf[2] = Valve.SnCode[0];
            protext.replyBuf[3] = Valve.SnCode[1];
            protext.replyBuf[4] = Valve.SnCode[2];
            protext.replyBuf[5] = Valve.SnCode[3];
            break;
        case RD_NOW_POS:
            protext.replyBuf[2] = 0x00;
            protext.replyBuf[3] = 0x00;
            protext.replyBuf[4] = 0x00;
            protext.replyBuf[5] = Valve.portCur;
            break;
        case SET_PROTOCAL:
            protext.replyBuf[2] = 0x00;
            protext.replyBuf[3] = 0x00;
            protext.replyBuf[4] = 0x00;
            if(protext.usartBuf[3]==MY_MODBUS || protext.usartBuf[3]==EXT_COMM)
                protext.replyBuf[5] = 0x00;
            else
                protext.replyBuf[5] = 0X01;
            break;
        case RD_NOW_STA:
            protext.replyBuf[2] = 0x00;
            protext.replyBuf[3] = 0x00;
            protext.replyBuf[4] = 0x00;
            if(Valve.bErr)
            {
                if(Valve.status==VALVE_ERR)
                    protext.replyBuf[5] = VALVE_ERR;
                else
                    protext.replyBuf[5] = 0x08;
            }
            else
            {
                if(Valve.status==VALVE_RUN_END)
                    protext.replyBuf[5] = 0x00;
                else
                    protext.replyBuf[5] = Valve.status;
            }
            break;
        default:
            protext.replyBuf[2] = 0x00;
            protext.replyBuf[3] = 0x00;
            protext.replyBuf[4] = 0x00;
            protext.replyBuf[5] = 0X01;
            break;
	}
	sdLen = REPLY_LENS;
    protext.sendCnt =  REPLY_LENS;
    checkSum = CommCheckSum(sdLen-1, protext.replyBuf);	// 获取CRC
    protext.replyBuf[sdLen-1] = checkSum;

    prInfo(syspara.comInfo, "\r\n s:");
    for(uint8 i=0; i<sdLen; i++)
    {
        prInfo(syspara.comInfo, " %02x", protext.replyBuf[i]);
    }
    TX_EN();
    MYDMA_Transmit_Enable(USART3, DMA1_Channel2, REPLY_LENS);
    MYDMA_Transmit_Enable(USART2, DMA1_Channel7, REPLY_LENS);
    
}


/*

*/
void NormalAction(void)
{
    uint32 Itemp=0;
    if(Valve.status==VALVE_RUN_END && !MotionStatus[AXSV])
    {
        Valve.dir = 0xff;		//
        syspara.protectTimeOut = 0;
    	if(!Valve.spd)
    	{
            I2CPageRead_Nbytes(ADDR_SPD, LEN_SPD, &Valve.spd);
            if(!Valve.spd || Valve.spd>SPD_LMT)
                Valve.spd = SPD_VALVE;
        }
        Itemp = protext.usartBuf[3];
        Itemp <<= 8;
        Itemp |= protext.usartBuf[4];
        Itemp <<= 8;
        Itemp |= protext.usartBuf[5];
        Itemp <<= 8;
        Itemp |= protext.usartBuf[6];
        if(Itemp!=0 && Itemp<=valveFix.fix.portCnt)
    	    Valve.portDes = Itemp;
        else
    	    Valve.bErrRetn = 0x01;
        syspara.sigRunTime = 0;
    }
}



/*

*/
void CWAction(void)
{
    uint32 Itemp=0;
    if(Valve.status==VALVE_RUN_END && !MotionStatus[AXSV])
    {
        Valve.dir = CW;		//
        syspara.protectTimeOut = 0;
    	if(!Valve.spd)
    	{
            I2CPageRead_Nbytes(ADDR_SPD, LEN_SPD, &Valve.spd);
            if(!Valve.spd || Valve.spd>SPD_LMT)
                Valve.spd = SPD_VALVE;
        }
        Itemp = protext.usartBuf[3];
        Itemp <<= 8;
        Itemp |= protext.usartBuf[4];
        Itemp <<= 8;
        Itemp |= protext.usartBuf[5];
        Itemp <<= 8;
        Itemp |= protext.usartBuf[6];

        if(Itemp<=valveFix.fix.portCnt)
    	    Valve.portDes = Itemp;
        else
    	    Valve.bErrRetn = 0x01;
    }
}


/*

*/
void CCWAction(void)
{
    uint32 Itemp=0;
    if(Valve.status==VALVE_RUN_END && !MotionStatus[AXSV])
    {
        Valve.dir = CCW;		//
        syspara.protectTimeOut = 0;
    	if(!Valve.spd)
    	{
            I2CPageRead_Nbytes(ADDR_SPD, LEN_SPD, &Valve.spd);
            if(!Valve.spd || Valve.spd>SPD_LMT)
                Valve.spd = SPD_VALVE;
        }
        Itemp = protext.usartBuf[3];
        Itemp <<= 8;
        Itemp |= protext.usartBuf[4];
        Itemp <<= 8;
        Itemp |= protext.usartBuf[5];
        Itemp <<= 8;
        Itemp |= protext.usartBuf[6];

        if(Itemp<=valveFix.fix.portCnt)
    	    Valve.portDes = Itemp;
        else
    	    Valve.bErrRetn = 0x01;
    }
}


/*

*/
void RstAction(void)
{
    Valve.retryTms = 0;
    Valve.status = VALVE_INITING;
    Valve.initStep = 0;
    Valve.portDes = 0;
    Valve.stpCnt = 0;
    Valve.bNewInit = 1;
    syspara.pwrOn = true;
    Valve.bErr = NONE_ERR;
    syspara.protectTimeOut = 0;
    I2CPageRead_Nbytes(ADDR_PORT_CNT, LEN_PORT_CNT, &valveFix.fix.portCnt);
    (valveFix.fix.portCnt&&valveFix.fix.portCnt>32)?(valveFix.fix.portCnt=10):(valveFix.fix.portCnt);
    I2CPageRead_Nbytes(ADDR_VALVE_FIX, LEN_VALVE_FIX, &Valve.fixOrg);
    if(Valve.fixOrg<=0)
    {
        Valve.fixOrg = 15;
    }
    I2CPageRead_Nbytes(ADDR_DIR_FIX, LEN_DIR_FIX, &valveFix.fix.dirGap);
}


/*

*/
void WRAction(void)
{
	Valve.SnCode[0] = protext.usartBuf[3]; 			// 设备地址
	Valve.SnCode[1] = protext.usartBuf[4];  			// 功能码
	Valve.SnCode[2] = protext.usartBuf[5];  			// 端口编号
	Valve.SnCode[3] = protext.usartBuf[6];  			// 补偿值
	Valve.SnCode[4] = 0X00;  			            // 补偿值
    I2CPageWrite_Nbytes(ADDR_SN, LEN_SN, Valve.SnCode);
    prInfo(syspara.typeInfo, "\r\n WR SN");
}

/*

*/
void RDAction(void)
{
    I2CPageRead_Nbytes(ADDR_SN, LEN_SN, Valve.SnCode);
    prInfo(syspara.typeInfo, "\r\n RD SN");
}

/*

*/
void ProtocalSet(void)
{
    if(protext.usartBuf[6]==MY_MODBUS || protext.usartBuf[6]==EXT_COMM)
    {
//        syspara.typeProtocal = ModbusPara.rBuf[3];
        I2CPageWrite_Nbytes(ADDR_PROTOCAL, LEN_PROTOCAL, &ModbusPara.rBuf[3]);
    }
}


/*
*/
 void UsartProcess(void)
{
    unsigned char i=0;
	if(protext.stepCnt==PROTOCOL_OK)
	{//usart4通讯处理
		if(protext.rxCount!=0)
		{
    	    prInfo(syspara.comInfo, "\r\n r:");
            for(i=0; i<protext.rxCount; i++)
        	{
                prInfo(syspara.comInfo, " %02x", protext.usartBuf[i]);
                #ifdef INFO_DEBUG
                if(errRecord.bPrint==false)
                    errRecord.cmdLast[i] = protext.usartBuf[i];
                #endif
            }
			if(CommCheckSum(RECEIVE_LENS-1, protext.usartBuf)==protext.usartBuf[RECEIVE_LENS-1])		//CRC校验成功
			{
                switch(P_COMMAND)
                {
                    case SHIFT_NML:
                        NormalAction();
                        break;
                    case SHIFT_DE:
                        CWAction();
                        break;
                    case SHIFT_IN:
                        CCWAction();
                        break;
                    case ACTN_RESET:
                        RstAction();
                        break;
                    case WR_CUST_DATA:
                        WRAction();
                        break;
                    case RD_CUST_DATA:
                        RDAction();
                        break;
                    case RD_NOW_STA:
                        break;
                    case RD_NOW_POS:
                        break;
                    case SET_PROTOCAL:
                        ProtocalSet();
                        break;
                    default:
                        break;
                }
		    }
 		}
		RxDataProcessEnd();
        AskStaProcess(0);
	}
}


void InfoErrPrint(void)
 {
    #ifdef INFO_DEBUG
    if(errRecord.bPrint==true)
    {
        errRecord.bPrint = false;
//        if(syspara.typeProtocal==MY_MODBUS)
//        {
//            printd("\r\n");
//            for(uint8 i=0; i<5; i++)
//                printd(" %02x", *(errRecord.cmdLast+i));
//            printd("\r\n");
//            for(uint8 i=0; i<12; i++)
//                printd(" %02x", *(errRecord.rplyLast+i));
//        }
//        else
        {
            printd("\r\n");
            for(uint8 i=0; i<RECEIVE_LENS; i++)
                printd(" %02x", *(errRecord.cmdLast+i));
            printd("\r\n");
            for(uint8 i=0; i<REPLY_LENS; i++)
                printd(" %02x", *(errRecord.rplyLast+i));
        }
    }
    #endif
}


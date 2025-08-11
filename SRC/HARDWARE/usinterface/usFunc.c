#define _USFUNC_GLOBALS_
#include "common.h"


char bRw;
unsigned char FuncIndex;
void (*FuncPtr)(char rw);          //函数功能指针

//-------------------------界面相关定制函数-------------------------//
/*

*/
void TermNone(char rw)
{
     prInfo(syspara.typeInfo, "\r\n Command \"%s\" invalid!", str.rcvStr);
}


/*

*/
void TermVR(char rw)
{
    int getInt;
    if(rw==READ_ACT)
    {
        prInfo(syspara.typeInfo, "\r\n %s", SOFT_VS);
    }
    else
    {
        unsigned char ret = FetchInt(2, 0,str.rcvStr, &getInt);
        if(ret)
        {
            return;
        }
        switch(getInt)
        {
            case 0:
                syspara.typeInfo = PR_NONE;
                break;
            case 1:
                syspara.typeInfo = PR_INFO;
                break;
            case 2:
                syspara.typeInfo = PR_DBG;
                break;
            case 3:
                syspara.typeInfo = PR_ALL;
                break;

        }
    }
}


/*

*/
void TermList(char rw)
{
    prInfo(syspara.typeInfo, "\r\n %s", S_LIST_M);
 }


/*

*/
void TermMap(char rw)
{
    char getChar[2][P_LEN+1];
    int getInt=0;
    int *getAddr=NULL;

    memset(getChar, 0, sizeof(getChar));
    if(rw==READ_ACT)
    {
        prInfo(syspara.typeInfo, "\r\n No para");
    }
    else
    {
        unsigned char ret = FetchChar(3, 0, str.rcvStr, *getChar);
        if(ret)
        {
            prInfo(syspara.typeInfo, "\r\n Err code %d", ret);
            return;
        }
        getAddr = (int *)strtohex(*getChar);
        if(**(getChar+1))
        {//判断是否有参数写入
            getInt = str2int(*(getChar+1));
            *getAddr = getInt;
            prInfo(syspara.typeInfo, "\r\n Wr 0x%08x to 0x%08x", getInt, getAddr);
        }
        else
        {//无参数，取指
            getInt = (int)(*getAddr);
            prInfo(syspara.typeInfo, "\r\n Rd 0x[%02x %02x %02x %02x] from 0x%08x",
                (char)(getInt>>24), (char)(getInt>>16), (char)(getInt>>8), (char)(getInt>>0), getAddr);
        }
    }
}


/*

*/
void TermIIC(char rw)
{
    if(rw==READ_ACT)
    {
        prInfo(syspara.typeInfo, "\r\n No para");
    }
    else
    {
        #if FLASH_CHK
        unsigned char rwBuf[4]={0, 0, 0, 0};
        int getInt=0;

        unsigned char ret = FetchInt(3, 0, str.rcvStr, &getInt);
        if(ret)
        {
            prInfo(syspara.typeInfo, "\r\n ERR%d", ret);
            return;
        }
        if(getInt)
        {
            *(rwBuf+0) = getInt>>8;
            *(rwBuf+1) = getInt;
            I2CPageWrite_Nbytes(0, 2, rwBuf);
            prInfo(syspara.typeInfo, "\r\n I2C write 0x%02x 0x%02x", *(rwBuf+0), *(rwBuf+1));
            *(rwBuf+2) = 0;
            *(rwBuf+3) = 0;
            I2CPageRead_Nbytes(0, 2, rwBuf+2);
            prInfo(syspara.typeInfo, "\r\n I2C read 0x%02x 0x%02x", *(rwBuf+2), *(rwBuf+3));
            if(*(rwBuf+0)==*(rwBuf+2) && *(rwBuf+1)==*(rwBuf+3))
                prInfo(syspara.typeInfo, "\r\n I2C R/W Succeed!");
            else
                prInfo(syspara.typeInfo, "\r\n I2C R/W Fail!");
        }
        else
        {
            *(rwBuf+0) = 0;
            *(rwBuf+1) = 0;
            I2CPageWrite_Nbytes(ADDR_BOARD_ID, LEN_BOARD_ID, rwBuf);
            prInfo(syspara.typeInfo, "\r\n Clear store id!");
        }
        #else
        unsigned short rwBuf[4]={0, 0, 0, 0};
        int getInt=0;
        unsigned char ret = FetchInt(3, 0, str.rcvStr, &getInt);
        if(ret)
        {
            prInfo(syspara.typeInfo, "\r\n ERR%d", ret);
            return;
        }
        if(getInt)
        {
            *(rwBuf+0) = getInt>>16;
            *(rwBuf+1) = getInt;
            STMFLASH_Write(FLASH_SAVE_ADDR, rwBuf, 2);
            prInfo(syspara.typeInfo, "\r\n write 0x%04x 0x%04x", *(rwBuf+0), *(rwBuf+1));
            *(rwBuf+2) = 0;
            *(rwBuf+3) = 0;
            STMFLASH_Read(FLASH_SAVE_ADDR, rwBuf+2, 2);
            prInfo(syspara.typeInfo, "\r\n read 0x%04x 0x%04x", *(rwBuf+2), *(rwBuf+3));
            if(*(rwBuf+0)==*(rwBuf+2) && *(rwBuf+1)==*(rwBuf+3))
                prInfo(syspara.typeInfo, "\r\n R/W Succeed!");
            else
                prInfo(syspara.typeInfo, "\r\n R/W Fail!");
        }
        else
        {
            *(rwBuf+0) = 0;
            *(rwBuf+1) = 0;
            STMFLASH_Read(ADDR_FLASH_ID, rwBuf, LEN_FLASH_ID);
            prInfo(syspara.typeInfo, "\r\n Factory set!");
        }
        #endif
    }
}


void TermCat(char rw)
{
    int getInt[2] = {0, 0};
    if(rw==READ_ACT)
    {
        prInfo(syspara.typeInfo, "\r\n Cat comm");
    }
    else
    {
        unsigned char ret = FetchInt(3, 0, str.rcvStr, getInt);
        prInfo(syspara.typeInfo, "\r\n Cat %d %d", *(getInt+0), *(getInt+1));
       if(ret)
        {
            prInfo(syspara.typeInfo, "\r\n Err code %d", ret);
            return;
        }
        switch(*(getInt+0))
        {
            case 1:

                break;
            case 2:

                break;
            case 3:

                break;
            case 4:

                break;
            default:
                break;
        }
    }
}


void TermFetch(char rw)
{
    int getInt[2] = {0, 0};
    if(rw==READ_ACT)
    {
        prInfo(syspara.typeInfo, "\r\n Cat comm");
    }
    else
    {
        unsigned char ret = FetchInt(3, 2, str.rcvStr, getInt);
        if(ret)
        {
            prInfo(syspara.typeInfo, "\r\n Err code %d", ret);
            return;
        }
        prInfo(syspara.typeInfo, "\r\n Fet %d %d", *(getInt+0), *(getInt+1));
        switch(*(getInt+0))
        {
            case 1:

                break;
            case 2:

                break;
            case 3:

                break;
            case 4:

                break;
            default:
                break;
        }
    }
}


/*

*/
void TermTs(char rw)
{
    int getInt[2]={0, 0};
//    uint8 buffer[2]={0,0};
    if(rw == READ_ACT)
    {
        prInfo(syspara.typeInfo, "%s", S_LIST_SH);
        prInfo(syspara.typeInfo, "%s", S_LIST_SBD);
        prInfo(syspara.typeInfo, "%s", S_LIST_SE);
    }
    else
    {
        unsigned char ret = FetchInt(2, 0, str.rcvStr, getInt);
        if(ret)
        {
            prInfo(syspara.typeInfo, "\r\n Err code %d", ret);
            return;
        }
        switch(getInt[0])
        {
            case 0:
                prInfo(syspara.typeInfo, "\r\n Move axis %d rounds", getInt[1]);
                if(!MotionStatus[AXSV])
                    AxisMoveRel(AXSV, -rdc.stepRound*getInt[1], accel[AXSV], decel[AXSV], speed[AXSV]);
                break;
            case 1:
                Valve.portDes = getInt[1];
                break;
            case 2:
                if(getInt[1]<=64)
                {
                    ModbusPara.mAddrs = getInt[1];
                    prInfo(syspara.typeInfo, "\r\n Set addrs %d", getInt[1]);
                    I2CPageWrite_Nbytes(ADDR_MODULE_NUM, LEN_MODULE_NUM, (uint8 *)&getInt[1]);
                }
                break;
            default:
                break;
        }
    }
}



/*

*/
void TermPos(char rw)
{
    int getInt[2] = {0};
    if(rw == READ_ACT)
    {
    }
    else
    {
        unsigned char ret = FetchInt(3, 0, str.rcvStr, getInt);
        if(ret)
        {
            prInfo(syspara.typeInfo, "\r\n Err code %d", ret);
            return;
        }
        if(Valve.status==VALVE_RUN_END && !MotionStatus[AXSV])
        {
            prInfo(syspara.typeInfo, "\r\n %d==>%d", Valve.portCur, getInt[0]);
            Valve.portDes = getInt[0];
            Valve.dir = getInt[1];
            syspara.protectTimeOut = 0;
        }
    }
}



/*

*/
void TermFixO(char rw)
{
    int getInt[2] = {0, 0};
    if(rw == READ_ACT)
    {
        I2CPageRead_Nbytes(ADDR_VALVE_FIX, LEN_VALVE_FIX, &Valve.fixOrg);
        prInfo(syspara.typeInfo, "\r\n FixO:%d", Valve.fixOrg);
    }
    else
    {
        unsigned char ret = FetchInt(4, 0, str.rcvStr, getInt);
        if(ret)
        {
            prInfo(syspara.typeInfo, "\r\n Err code %d", ret);
            return;
        }
        Valve.fixOrg = getInt[0];
        I2CPageWrite_Nbytes(ADDR_VALVE_FIX, LEN_VALVE_FIX, &Valve.fixOrg);
        prInfo(syspara.typeInfo, "\r\n set FixO:%d", Valve.fixOrg);
    }
}

/*

*/
void TermFixD(char rw)
{
    int getInt[2] = {0, 0};
    if(rw == READ_ACT)
    {
        I2CPageRead_Nbytes(ADDR_DIR_FIX, LEN_DIR_FIX, &valveFix.fix.dirGap);
        prInfo(syspara.typeInfo, "\r\n Dir:%d", valveFix.fix.dirGap);
    }
    else
    {
        unsigned char ret = FetchInt(4, 0, str.rcvStr, getInt);
        if(ret)
        {
            prInfo(syspara.typeInfo, "\r\n Err code %d", ret);
            return;
        }
        valveFix.fix.dirGap = getInt[0];
        I2CPageWrite_Nbytes(ADDR_DIR_FIX, LEN_DIR_FIX, &valveFix.fix.dirGap);
        prInfo(syspara.typeInfo, "\r\n set Dir:%d", valveFix.fix.dirGap);
    }
}



/*

*/
void TermAddr(char rw)
{
    int getInt=0;
    if(rw == READ_ACT)
    {
        I2CPageRead_Nbytes(ADDR_MODULE_NUM, LEN_MODULE_NUM, (uint8*)&getInt);
        prInfo(syspara.typeInfo, "\r\n Addr:%d", getInt);
    }
    else
    {
        unsigned char ret = FetchInt(4, 0, str.rcvStr, &getInt);
        if(ret)
        {
            prInfo(syspara.typeInfo, "\r\n Err code %d", ret);
            return;
        }
        if(getInt<=64||getInt==255)
        {
            I2CPageWrite_Nbytes(ADDR_MODULE_NUM, LEN_MODULE_NUM, (uint8*)&getInt);
            ModbusPara.mAddrs = getInt;
        }
    }
}


/*

*/
void TermInt(char rw)
{
    int getInt=0;
    if(rw == READ_ACT)
    {
    }
    else
    {
        unsigned char ret = FetchInt(3, 0, str.rcvStr, &getInt);
        if(ret)
        {
            prInfo(syspara.typeInfo, "\r\n Err code %d", ret);
            return;
        }
        if(getInt&&getInt<=255)
        {
            prInfo(syspara.typeInfo, "\r\n set INT to %d", getInt);
            intCtrl = getInt;
            I2CPageWrite_Nbytes(ADDR_INTVL, LEN_INTVL, &intCtrl);
        }
    }
}

/*

*/
void TermSpd(char rw)
{
    int getInt=0;
    if(rw == READ_ACT)
    {
        I2CPageRead_Nbytes(ADDR_SPD, LEN_SPD, &Valve.spd);
        prInfo(syspara.typeInfo, "\r\n read Spd %d", Valve.spd);
    }
    else
    {
        unsigned char ret = FetchInt(3, 0, str.rcvStr, &getInt);
        if(ret)
        {
            prInfo(syspara.typeInfo, "\r\n Err code %d", ret);
            return;
        }
        if(getInt&&getInt<=100)
        {
            prInfo(syspara.typeInfo, "\r\n set Spd to %d", getInt);
            I2CPageWrite_Nbytes(ADDR_SPD, LEN_SPD, (uint8*)&getInt);
        }
    }
}


/*

*/
void TermSN(char rw)
{
    int getInt[LEN_SN]={0};
    if(rw == READ_ACT)
    {
        I2CPageRead_Nbytes(ADDR_SN, LEN_SN, Valve.SnCode);
        prInfo(syspara.typeInfo, "\r\n read sn:");
        for(uint8 i=0; i<10; i++)
            prInfo(syspara.typeInfo, " %02x", Valve.SnCode[i]);

    }
    else
    {
        unsigned char ret = FetchInt(2, 0, str.rcvStr, getInt);
        if(ret)
        {
            prInfo(syspara.typeInfo, "\r\n Err code %d", ret);
            return;
        }

        prInfo(syspara.typeInfo, "\r\n set sn code");
        I2CPageWrite_Nbytes(ADDR_SN, LEN_SN, (uint8*)getInt);
    }
}

/*

*/
void TermProtocal(char rw)
{
    int getInt=0;
    if(rw == READ_ACT)
    {
        I2CPageRead_Nbytes(ADDR_PROTOCAL, LEN_PROTOCAL, &syspara.typeProtocal);
        if(getInt==MY_MODBUS || getInt==EXT_COMM)
        {
            prInfo(syspara.typeInfo, "\r\n now protocal is");
            if(syspara.typeProtocal==MY_MODBUS)
                prInfo(syspara.typeInfo, "\r\n MODBUS");
            else
                prInfo(syspara.typeInfo, "\r\n EXT PROTCAL");
        }
        else
        {
            prInfo(syspara.typeInfo, "\r\n read wrong type");
        }
    }
    else
    {
        unsigned char ret = FetchInt(5, 0, str.rcvStr, &getInt);
        if(ret)
        {
            prInfo(syspara.typeInfo, "\r\n Err code %d", ret);
            return;
        }
        if(getInt==MY_MODBUS || getInt==EXT_COMM)
        {
            syspara.typeProtocal = getInt;
            prInfo(syspara.typeInfo, "\r\n set protocal to");
            if(syspara.typeProtocal==MY_MODBUS)
                prInfo(syspara.typeInfo, "\r\n MODBUS");
            else
                prInfo(syspara.typeInfo, "\r\n EXT PROTCAL");
            I2CPageWrite_Nbytes(ADDR_PROTOCAL, LEN_PROTOCAL, &syspara.typeProtocal);
        }
        else
        {
            prInfo(syspara.typeInfo, "\r\n wrong type");
        }
    }
}



/*

*/
void TermBaud(char rw)
{
    int getInt = 0;
    if(rw == READ_ACT)
    {
    }
    else
    {
        unsigned char ret = FetchInt(3, 0, str.rcvStr, &getInt);
        if(ret)
        {
            prInfo(syspara.typeInfo, "\r\n Err code %d", ret);
            return;
        }
        if(getInt%9600)
        {
            prInfo(syspara.typeInfo, "\r\n baud rate %d not exist", getInt);
            return;
        }
        if(getInt==9600)
        {
            prInfo(syspara.typeInfo, "\r\n set baud rate to %d", getInt);
            syspara.bdrate = 1;
        }
        else if(getInt==19200)
        {
            prInfo(syspara.typeInfo, "\r\n set baud rate to %d", getInt);
            syspara.bdrate = 2;
        }
        I2CPageWrite_Nbytes(ADDR_BAUD, LEN_BAUD, &syspara.bdrate);
    }
}

/*

*/
void TermPulse(char rw)
{
    if(rw == READ_ACT)
    {
        if(syspara.bRdPulse==false)
            syspara.bRdPulse = true;
        else
            syspara.bRdPulse = false;
        I2CPageWrite_Nbytes(ADDR_RDP, LEN_RDP, &syspara.bRdPulse);
    }
}

/*

*/
void TermScan(char rw)
{
    if(bRw == READ_ACT)
    {
        if(!MotionStatus[AXSV])
        {
            sig.stpScan = 100;
            sig.blockNum = 0;
            sig.gapNum = 0;
            prInfo(syspara.typeInfo, "\r\n 开始扫描");
        }
        else
        {
            prInfo(syspara.typeInfo, "\r\n 电机繁忙，命令未执行");
        }
    }
}



/*

*/
void TermRDCR(char rw)
{
    int getInt = 0;
    if(rw == READ_ACT)
    {
        I2CPageRead_Nbytes(ADDR_RDC_RATE, LEN_RDC_RATE, &rdc.rate);
        prInfo(syspara.typeInfo, "\r\n read rate %d", rdc.rate);
    }
    else
    {
        unsigned char ret = FetchInt(4, 0, str.rcvStr, &getInt);
        if(ret)
        {
            prInfo(syspara.typeInfo, "\r Err code %d", ret);
            return;
        }
        prInfo(syspara.typeInfo, "\r\n set rate %d", getInt);
        rdc.rate = getInt;
        I2CPageWrite_Nbytes(ADDR_RDC_RATE, LEN_RDC_RATE, &rdc.rate);
    }
}

/*

*/
void TermCnt(char rw)
{
    int getInt = 0;
    if(rw == WRITE_ACT)
    {
        unsigned char ret = FetchInt(3, 0, str.rcvStr, &getInt);
        if(ret)
        {
            prInfo(syspara.typeInfo, "\r Err code %d", ret);
            return;
        }
        valveFix.fix.portCnt = getInt;
        I2CPageWrite_Nbytes(ADDR_PORT_CNT, LEN_PORT_CNT, &valveFix.fix.portCnt);
        prInfo(syspara.typeInfo, "\r\n portCnt%d", valveFix.fix.portCnt);
    }
}

/*

*/
void TermHalf(char rw)
{
    int getInt = 0;
    if(rw == WRITE_ACT)
    {
        unsigned char ret = FetchInt(4, 0, str.rcvStr, &getInt);
        if(ret)
        {
            prInfo(syspara.typeInfo, "\r Err code %d", ret);
            return;
        }
        prInfo(syspara.typeInfo, "\r\n set half %d", getInt);
        Valve.bHalfSeal = getInt;
        I2CPageWrite_Nbytes(ADDR_HALF_SEAL, LEN_HALF_SEAL, &Valve.bHalfSeal);
    }
    else
    {
        I2CPageRead_Nbytes(ADDR_HALF_SEAL, LEN_HALF_SEAL, &Valve.bHalfSeal);
        prInfo(syspara.typeInfo, "\r\n read half %d", Valve.bHalfSeal);
    }
}


/*

*/
void TermMotor(char rw)
{
    int getInt = 0;
    if(rw == WRITE_ACT)
    {
        unsigned char ret = FetchInt(5, 0, str.rcvStr, &getInt);
        if(ret)
        {
            prInfo(syspara.typeInfo, "\r Err code %d", ret);
            return;
        }
        getInt *= rdc.stepP1dgr;
        if(!MotionStatus[AXSV])
            AxisMoveAbs(AXSV, getInt, accel[AXSV], decel[AXSV], speed[AXSV]);
        prInfo(syspara.typeInfo, "\r\n motor %d", getInt);
    }
}

/*

*/
void TermRST(char rw)
{
    Valve.retryTms = 0;
    syspara.pwrOn = true;
    Valve.status = VALVE_INITING;
    Valve.initStep = 0;
    Valve.portDes = 0;
    Valve.stpCnt = 0;
    Valve.bErr = 0;
    Valve.bNewInit = 1;
    syspara.protectTimeOut = 0;
}


_TAB_T TermTab[]=
{
    {0,     (*TermNone)},
    {1,     (*TermList)},
    {2,     (*TermVR)},
    {3,     (*TermMap)},
    {4,     (*TermIIC)},
    {5,     (*TermTs)},
    {6,     (*TermCat)},
    {7,     (*TermFetch)},
    {8,     (*TermPos)},
    {9,     (*TermFixO)},
    {10,    (*TermFixD)},
    {11,    (*TermAddr)},
    {12,    (*TermInt)},
    {13,    (*TermSpd)},
    {14,    (*TermSN)},
    {15,    (*TermProtocal)},
    {16,    (*TermBaud)},
    {17,    (*TermPulse)},
    {18,    (*TermScan)},
    {19,    (*TermRDCR)},
    {20,    (*TermCnt)},
    {21,    (*TermMotor)},
    {22,    (*TermHalf)},
    {23,    (*TermRST)},
};


/*

*/
void ChRUN(char *cmdName)
{
    if(!strcasecmp(cmdName, CMD_LIST))
    {
        FuncIndex = 1;
    }
    else if(!strncasecmp(cmdName, CMD_VR, 2))
    {
        (!strcasecmp(cmdName, CMD_VR))?(bRw = READ_ACT):(bRw = WRITE_ACT);
        FuncIndex = 2;
    }
    else if(!strcasecmp(cmdName, CMD_MAP) || !strncasecmp(cmdName, CMD_MAP, 3))
    {
        (!strcasecmp(cmdName, CMD_MAP))?(bRw = READ_ACT):(bRw = WRITE_ACT);
        FuncIndex = 3;
    }
    else if(!strcasecmp(cmdName, CMD_IIC) || !strncasecmp(cmdName, CMD_IIC, 3))
    {
        (!strcasecmp(cmdName, CMD_IIC))?(bRw = READ_ACT):(bRw = WRITE_ACT);
        FuncIndex = 4;
    }
    else if(!strncasecmp(cmdName, CMD_TS, 2))
    {
        (!strcasecmp(cmdName, CMD_TS))?(bRw = READ_ACT):(bRw = WRITE_ACT);
        FuncIndex = 5;
    }
    else if(!strcasecmp(cmdName, CMD_CAT) || !strncasecmp(cmdName, CMD_CAT, 3))
    {
        (!strcasecmp(cmdName, CMD_CAT))?(bRw = READ_ACT):(bRw = WRITE_ACT);
        FuncIndex = 6;
    }
    else if(!strcasecmp(cmdName, CMD_FET) || !strncasecmp(cmdName, CMD_FET, 3))
    {
        (!strcasecmp(cmdName, CMD_FET))?(bRw = READ_ACT):(bRw = WRITE_ACT);
        FuncIndex = 7;
    }
    else if(!strcasecmp(cmdName, CMD_POS) || !strncasecmp(cmdName, CMD_POS, 3))
    {
        (!strcasecmp(cmdName, CMD_POS))?(bRw = READ_ACT):(bRw = WRITE_ACT);
        FuncIndex = 8;
    }
    else if(!strcasecmp(cmdName, "FIXO") || !strncasecmp(cmdName, "FIXO", 4))
    {
        (!strcasecmp(cmdName, "FIXO"))?(bRw = READ_ACT):(bRw = WRITE_ACT);
        FuncIndex = 9;
    }
    else if(!strcasecmp(cmdName, "FIXD") || !strncasecmp(cmdName, "FIXD", 4))
    {
        (!strcasecmp(cmdName, "FIXD"))?(bRw = READ_ACT):(bRw = WRITE_ACT);
        FuncIndex = 10;
    }
    else if(!strcasecmp(cmdName, CMD_ADDR) || !strncasecmp(cmdName, CMD_ADDR, 4))
    {
        (!strcasecmp(cmdName, CMD_ADDR))?(bRw = READ_ACT):(bRw = WRITE_ACT);
        FuncIndex = 11;
    }
    else if(!strcasecmp(cmdName, "INT") || !strncasecmp(cmdName, "INT", 3))
    {
        (!strcasecmp(cmdName, "INT"))?(bRw = READ_ACT):(bRw = WRITE_ACT);
        FuncIndex = 12;
    }
    else if(!strcasecmp(cmdName, "SPD") || !strncasecmp(cmdName, "SPD", 3))
    {
        (!strcasecmp(cmdName, "SPD"))?(bRw = READ_ACT):(bRw = WRITE_ACT);
        FuncIndex = 13;
    }
    else if(!strcasecmp(cmdName, "SN") || !strncasecmp(cmdName, "SN", 2))
    {
        (!strcasecmp(cmdName, "SN"))?(bRw = READ_ACT):(bRw = WRITE_ACT);
        FuncIndex = 14;
    }
    else if(!strcasecmp(cmdName, "PRTCL") || !strncasecmp(cmdName, "PRTCL", 5))
    {
        (!strcasecmp(cmdName, "PRTCL"))?(bRw = READ_ACT):(bRw = WRITE_ACT);
        FuncIndex = 15;
    }
    else if(!strcasecmp(cmdName, "BDR") || !strncasecmp(cmdName, "BDR", 3))
    {
        (!strcasecmp(cmdName, "BDR"))?(bRw = READ_ACT):(bRw = WRITE_ACT);
        FuncIndex = 16;
    }
    else if(!strcasecmp(cmdName, "RDP") || !strncasecmp(cmdName, "RDP", 3))
    {
        (!strcasecmp(cmdName, "RDP"))?(bRw = READ_ACT):(bRw = WRITE_ACT);
        FuncIndex = 17;
    }
    else if(!strcasecmp(cmdName, "SCAN") || !strncasecmp(cmdName, "SCAN", 4))
    {
        (!strcasecmp(cmdName, "SCAN"))?(bRw = READ_ACT):(bRw = WRITE_ACT);
        FuncIndex = 18;
    }
    else if(!strcasecmp(cmdName, "RDCR") || !strncasecmp(cmdName, "RDCR", 4))
    {
        (!strcasecmp(cmdName, "RDCR"))?(bRw = READ_ACT):(bRw = WRITE_ACT);
        FuncIndex = 19;
    }
    else if(!strcasecmp(cmdName, "CNT") || !strncasecmp(cmdName, "CNT", 3))
    {
        (!strcasecmp(cmdName, "CNT"))?(bRw = READ_ACT):(bRw = WRITE_ACT);
        FuncIndex = 20;
    }
    else if(!strcasecmp(cmdName, "MOTOR") || !strncasecmp(cmdName, "MOTOR", 5))
    {
        (!strcasecmp(cmdName, "MOTOR"))?(bRw = READ_ACT):(bRw = WRITE_ACT);
        FuncIndex = 21;
    }
    else if(!strcasecmp(cmdName, "HALF") || !strncasecmp(cmdName, "HALF", 4))
    {
        (!strcasecmp(cmdName, "HALF"))?(bRw = READ_ACT):(bRw = WRITE_ACT);
        FuncIndex = 22;
    }
    else if(!strcasecmp(cmdName, "RESET") || !strncasecmp(cmdName, "RESET", 5))
    {
        (!strcasecmp(cmdName, "RESET"))?(bRw = READ_ACT):(bRw = WRITE_ACT);
        FuncIndex = 23;
    }

    else
    {
        FuncIndex = 0;
    }
    FuncPtr = TermTab[FuncIndex].CurrOp;
    (*FuncPtr)(bRw);
}





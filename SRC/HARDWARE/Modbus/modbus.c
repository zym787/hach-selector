#define _MODBUS_GLOBALS_
#include "common.h"

void ModbusInit(void)
{
	unsigned char cnt;

    RCC->APB2ENR |= (RCC_APB2Periph_GPIOB);
    GPIOB->CRL &= (GPIO_Crl_P1);
    GPIOB->CRL |= (GPIO_Mode_Out_PP_50MHz_P1);

	RX_EN();			                        // ����Ϊ����ģʽ
	I2CPageRead_Nbytes(ADDR_BAUD, LEN_BAUD, &syspara.bdrate);
    if(syspara.bdrate==2)
    {
        Usart2_Init(36, BAUD_RATE_19200);				//���ڳ�ʼ��Ĭ��Ϊ19200
    	delay_ms(100);
    	Usart3_Init(36, BAUD_RATE_19200);	            // ����2 485��ʼ��Ϊ19200
    	delay_ms(100);
    	TIM3_Init(MODBUS_TIME_19200,71);	                // 45us--0.45ms
    }
    else
    {
        Usart2_Init(36, BAUD_RATE_9600);				//���ڳ�ʼ��Ĭ��Ϊ9600
    	delay_ms(100);
    	Usart3_Init(36, BAUD_RATE_9600);	        // ����2 485��ʼ��Ϊ9600
    	delay_ms(100);
    	TIM3_Init(MODBUS_TIME_9600,71);	                // 45us--0.45ms
    }
	delay_ms(100);

	// ��������
	ModbusPara.sRUN =  MB_IDEL;
	ModbusPara.sERR = ERR_NOT;
	ModbusPara.times = 0;
	ModbusPara.rCnt = 0;
	ModbusPara.HostRept = 0;
//	ModbusPara.mAddrs = MB_SALVE_DEF_ADDR;

	for(cnt = 0 ;cnt < LENGTH_MB_DATA ;cnt++)
	{
		ModbusPara.rBuf[cnt] = 0;
		ModbusPara.tBuf[cnt] = 0;
	}

	for(cnt = 0 ;cnt <LENGTH_COIL_REG ;cnt++)
		CoilBitPara[cnt] = 0;

	for(cnt = 0 ;cnt <LENGTH_DiscreteREG ;cnt++)
		DiscreteBitPara[cnt] = 0;

	for(cnt = 0 ;cnt <LENGTH_HoldingREG ;cnt++)
		HoldingREGPara[cnt] = 0;

	for(cnt = 0 ;cnt <LENGTH_InputREG ;cnt++)
		InputREGPara[cnt] = 0;
}


void ModbusTimesProcess(void)
{
	if(ModbusPara.sRUN&MB_BUSY)
	{// ���߼��
		if(ModbusPara.times < BUS_IDLE_TIME)
		{
			ModbusPara.times++;
		}
		else
		{
			//���߽�����У�֡������ʼ
			if(ModbusPara.sRUN == MB_RECIVE_ERR)
			{// ���չ����� �г������ݴ洢�ռ��������ʱ�䳬��T1.5
				ModbusPara.sERR = ERR_MB_DEVICE;
				ModbusPara.sRUN =  MB_IDEL;
			}
			else if(ModbusPara.sRUN == MB_NO_RESPONSE)
			{
				ModbusPara.sRUN = MB_IDEL;
			}
			else if(ModbusPara.sRUN == MB_RECIVE)
			{
				ModbusPara.sRUN =  MB_RECIVE_END;
			}
		}
	}
}


void ModbusSend(unsigned char length)
{
	unsigned char cnt;

	TX_EN();
	if(length)
	{
		ModbusPara.sRUN = MB_SEND;
		for(cnt=0; cnt < length; cnt++)
		{
			while((USART3->SR&0X40)==0);        //�ȴ����ͽ���
			USART3->DR = ModbusPara.tBuf[cnt];
			ModbusPara.times = 0;               //���¼�ʱ��
		}
	}
	while((USART3->SR&0X40)==0);                //�ȴ����ͽ���
	RX_EN();

	if(length)
	{
		ModbusPara.sRUN = MB_SEND;
		for(cnt=0; cnt < length; cnt++)
		{
			while((USART2->SR&0X40)==0);        //�ȴ����ͽ���
			USART2->DR = ModbusPara.tBuf[cnt];
			ModbusPara.times = 0;               //���¼�ʱ��
		}
	}
	while((USART2->SR&0X40)==0);                //�ȴ����ͽ���
	ModbusPara.sRUN = MB_IDEL;
	ModbusPara.rCnt = 0;
}

void ModbusReceive(unsigned char res)
{
	ModbusPara.times = 0;  //���¼�ʱ
	if(ModbusPara.sRUN==MB_IDEL && !ModbusPara.rCnt)
	{// ���в������ݴ�����������Խ����µĽ���
		if(ModbusPara.mAddrs==res || res==MB_Broadcast_ADDR)
		{// ��ʼ��������
			ModbusPara.sRUN = MB_RECIVE;
			ModbusPara.sERR = ERR_NOT;
    		ModbusPara.rCnt = 1;
    		ModbusPara.rBuf[0] = res;
		}
		else
		{// �Ǳ��豸��ַ����ǵ�ǰ��ѯ���豸���ҷǹ㲥��ַ
			ModbusPara.sRUN = MB_NO_RESPONSE;
			ModbusPara.sERR = ERR_MB_DEVICE;
		}
	}
	else if(ModbusPara.sRUN==MB_RECIVE)
	{
		// ���������ߴ�����̳���ʱ��������T1.5�������ڽ���
		if(ModbusPara.rCnt<LENGTH_MB_DATA && ModbusPara.times<FRAME_ERR_TIME)
			ModbusPara.rBuf[ModbusPara.rCnt] = res;
		else
            ModbusPara.sRUN = MB_RECIVE_ERR;

    	++ModbusPara.rCnt;
	}
}

void Modbus_ERROR(void)
{
	unsigned short 	temp16;

	if(ModbusPara.sERR==ERR_MB_FUN || ModbusPara.sERR==ERR_MB_ADDR || ModbusPara.sERR==ERR_MB_DATA)
	{// ��ģʽ,������Ӧ����
		ModbusPara.tBuf[0] = ModbusPara.rBuf[0];			// �豸��ַ
		ModbusPara.tBuf[1] = ModbusPara.rBuf[1]|0X80;	    // ������| 0X80
		ModbusPara.tBuf[2] = ModbusPara.sERR;			    // �������
		temp16 = ModbusCRC16(&ModbusPara.tBuf[0], 3);	    // ��ȡCRC
		ModbusPara.tBuf[3] = temp16 >> 8;
		ModbusPara.tBuf[4] = temp16 ;
		if(ModbusPara.tBuf[0] != MB_Broadcast_ADDR)
		{
            ModbusSend(5);
		}
	}
	ModbusPara.sERR = ERR_NOT;
}


void MB_ReadHoldingRegisters(void)
{ // ������3
	unsigned short reg_num;
    unsigned char dvc_addr, func_num, byteCount;

	dvc_addr = ModbusPara.rBuf[0];		//ģ���ַ
    if(dvc_addr==255)
    {
    	unsigned short addr_data,reg_num;
    	unsigned char  k,byteCount; 
    	addr_data = ModbusPara.rBuf[2];		//��ʼ��ַ
    	addr_data <<= 8;
    	addr_data += ModbusPara.rBuf[3];
    	reg_num = ModbusPara.rBuf[4];		//�������
    	reg_num <<= 8;
    	reg_num += ModbusPara.rBuf[5];
    	if(reg_num >= 1 && reg_num <= SUM_HoldingREG_WORD)
    	{// ���ּĴ��������ж�OK
    		if(addr_data < SUM_HoldingREG_WORD && (addr_data + reg_num) <= SUM_HoldingREG_WORD)
    		{// ���ּĴ�����ַ��Χ�ж�OK,��ʼ��Ӧ����
    			byteCount = reg_num *2;			// �ֽ���= 	�������*2
    			reg_num = addr_data + byteCount ;	//����  ���ּĴ�����= ��ʼ���ּĴ�����+ ���� 
    			for (k = 0 ; k < byteCount ; k++) 
    			{ // ��ȡ���豸��Ҫ�ı��ּĴ�������
    				ModbusPara.tBuf[k + 3] =  HoldingREGPara[addr_data];
    				ModbusPara.tBuf[k + 3] = 0x02;		// test
    				addr_data++;
    				if(addr_data >= LENGTH_HoldingREG|| addr_data >= reg_num )
    				{// ���ݴ�������г���
    					ModbusPara.sERR= ERR_MB_DATA;
    					break;
    				}
    			}
    			ModbusPara.tBuf[0] = ModbusPara.rBuf[0]; 			// �豸��ַ
    			ModbusPara.tBuf[1] = ModbusPara.rBuf[1];  			// ������
    			ModbusPara.tBuf[2] = byteCount; 					// �ֽ���
    			byteCount += 3; 
    			reg_num = ModbusCRC16( &ModbusPara.tBuf[0], byteCount);	// ��ȡCRC
    			ModbusPara.tBuf[byteCount] = reg_num >> 8;
    			byteCount ++; 
    			ModbusPara.tBuf[byteCount] = reg_num ;
    			byteCount ++; 
    			if(ModbusPara.tBuf[0] != MB_Broadcast_ADDR)  ModbusSend(byteCount );
    		}
    		else
    		{// ����ϵͳ�涨���ּĴ�����ַ��Χ���߶�ȡ��������ʼ��ַ��ƥ�����
    			ModbusPara.sERR= ERR_MB_ADDR;
    		}	
    	}
    	else
    	{// ����ϵͳ�涨���ּĴ���������
    		ModbusPara.sERR= ERR_MB_DATA;
    	}
    }
    else
    {    
    	func_num = ModbusPara.rBuf[2];		//�˿ڱ��
    	if(ModbusPara.mAddrs==dvc_addr)
    	{// ģ���ַ�ж�OK
    		if(func_num==0 || func_num==1 || func_num==2 || func_num==3 || func_num==4 || func_num==5 || func_num==8)
    		{// ͨ������ж�OK,��ʼ��Ӧ����
    			ModbusPara.tBuf[0] = ModbusPara.rBuf[0]; 			// �豸��ַ
    			ModbusPara.tBuf[1] = ModbusPara.rBuf[1];  			// ������
    			ModbusPara.tBuf[2] = ModbusPara.rBuf[2];  			// ������
    			if(func_num==0)
        		{
        			ModbusPara.tBuf[3] = Valve.status; 		        // ģ��״̬
        			ModbusPara.tBuf[4] = Valve.portCur;             // ͨ�����
        			ModbusPara.tBuf[5] = ModbusPara.mAddrs;         // ģ���ַ
        			ModbusPara.tBuf[6] = valveFix.fix.portCnt;      // ģ��ͨ����
        			ModbusPara.tBuf[7] = Valve.fixOrg;              // ԭ�㲹��ֵ
        			ModbusPara.tBuf[8] = valveFix.fix.dirGap;       // ���򲹳�ֵ
        			ModbusPara.tBuf[9] = Valve.spd;                 // �ٶ�
        			byteCount = 10;
        	    }
        	    else if(func_num==1)
        		{
                    ModbusPara.tBuf[3] = Valve.portCur;             // ͨ�����
                    byteCount = 4;
        	    }
        	    else if(func_num==2)
        		{
        			ModbusPara.tBuf[3] = ModbusPara.mAddrs;         // ģ���ַ
                    byteCount = 4;
        	    }
        	    else if(func_num==3)
        		{
        			ModbusPara.tBuf[3] = SOFT_VER;                  // ģ��汾
                    byteCount = 4;
        	    }
        	    else if(func_num==4)
        		{
        			ModbusPara.tBuf[3] = Valve.fixOrg;              // ԭ�㲹��ֵ
                    byteCount = 4;
        	    }
        	    else if(func_num==5)
        		{
        			ModbusPara.tBuf[3] = valveFix.fix.dirGap;       // ����ֵ
                    byteCount = 4;
        	    }
        	    else if(func_num==8)
        		{
                    ModbusPara.tBuf[3] = Valve.SnCode[0];
                    ModbusPara.tBuf[4] = Valve.SnCode[1];
                    ModbusPara.tBuf[5] = Valve.SnCode[2];
                    ModbusPara.tBuf[6] = Valve.SnCode[3];
                    ModbusPara.tBuf[7] = Valve.SnCode[4];
                    byteCount = 8;
        	    }

    			reg_num = ModbusCRC16( &ModbusPara.tBuf[0], byteCount);	// ��ȡCRC
    			ModbusPara.tBuf[byteCount] = reg_num >> 8;
    			byteCount++;
    			ModbusPara.tBuf[byteCount] = reg_num ;
    			byteCount++;

    			if(ModbusPara.tBuf[0] != MB_Broadcast_ADDR)
                   ModbusSend(byteCount);
                prInfo(syspara.comInfo, "\r\n s:");
                for(uint8 i=0; i<byteCount; i++)
                {
                    prInfo(syspara.comInfo, " %02x", ModbusPara.tBuf[i]);
//                    #ifdef INFO_DEBUG
//                    if((Valve.status==VALVE_ERR && errRecord.bPrint==false))
//                    {
//                        errRecord.rplyLast[i] = ModbusPara.tBuf[i];
//                        if(i==byteCount-1)
//                        {
//                            errRecord.bPrint = true;
//                        }
//                    }
//                    #endif
                }
    		}
    		else
    		{// ����ϵͳ�涨���ּĴ�����ַ��Χ���߶�ȡ��������ʼ��ַ��ƥ�����
    			ModbusPara.sERR = ERR_MB_DATA;
    		}
    	}
    	else
    	{// ����ϵͳ�涨���ּĴ���������
    		ModbusPara.sERR = ERR_MB_ADDR;
    	}
    }
}

void MB_PresetSingleHoldingRegister(void)
{// ������6
	unsigned short reg_num;
    unsigned char dvc_addr, func_num, port_num, byteCount,facBuf[2]={0};

	dvc_addr = ModbusPara.rBuf[0];		//ģ���ַ
	func_num = ModbusPara.rBuf[2];		//�˿ڱ��
	port_num = ModbusPara.rBuf[3];		//�˿ڱ��
	if(ModbusPara.mAddrs==dvc_addr)
	{// ģ���ַ�ж�OK
	    if(func_num==0)
	    {
    		if(port_num && port_num<=valveFix.fix.portCnt)
    		{// ͨ������ж�OK,��ʼ��Ӧ����
                if(Valve.status==VALVE_RUN_END && !MotionStatus[AXSV])
                {
                    syspara.protectTimeOut = 0;
                	Valve.portDes = port_num;
                	Valve.dir = 0xff;		// ����
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
                }
    		}
	    }
        else if(func_num==1)
        {
    		if(port_num<=ADDR_MAX)
    		{// д���ַ����ж�OK,��ʼ��Ӧ����
                I2CPageWrite_Nbytes(ADDR_MODULE_NUM, LEN_MODULE_NUM, &port_num);
    		}
        }
        else if(func_num==2)
        {
    		if(port_num && port_num<=BYTE_RANGE)
    		{// д��ԭ�㲹��
                I2CPageWrite_Nbytes(ADDR_VALVE_FIX, LEN_VALVE_FIX, &port_num);
    		}
        }
        else if(func_num==3)
        {
    		if(port_num && port_num<=BYTE_RANGE)
    		{// д�뷽�򲹳�
                I2CPageWrite_Nbytes(ADDR_DIR_FIX, LEN_DIR_FIX, &port_num);
    		}
        }
        else if(func_num==6)
        {// ��λָ��
            Valve.retryTms = 0;
            Valve.status = VALVE_INITING;
            Valve.initStep = 0;
            Valve.portDes = 0;
            Valve.stpCnt = 0;
            Valve.bNewInit = 1;
            syspara.pwrOn = true;
            Valve.bErr = 0;
            syspara.protectTimeOut = 0;
            VALVE_ENA = ENABLE;
            I2CPageRead_Nbytes(ADDR_PORT_CNT, LEN_PORT_CNT, &valveFix.fix.portCnt);
            (valveFix.fix.portCnt&&valveFix.fix.portCnt>32)?(valveFix.fix.portCnt=10):(valveFix.fix.portCnt);
            I2CPageRead_Nbytes(ADDR_VALVE_FIX, LEN_VALVE_FIX, &Valve.fixOrg);
            if(Valve.fixOrg<=0)
            {
                Valve.fixOrg = 15;
            }
            I2CPageRead_Nbytes(ADDR_DIR_FIX, LEN_DIR_FIX, &valveFix.fix.dirGap);
        }
        else if(func_num==7)
        {// ��������ָ��
            facBuf[0] = 0x66;
            facBuf[1] = 0x66;
            I2CPageWrite_Nbytes(ADDR_BOARD_ID, LEN_BOARD_ID, facBuf);
        }
        else if(func_num==8)
        {// ������ָ��
			Valve.SnCode[0] = ModbusPara.rBuf[3]; 			// �豸��ַ
			Valve.SnCode[1] = ModbusPara.rBuf[4];  			// ������
			Valve.SnCode[2] = ModbusPara.rBuf[5];  			// �˿ڱ��
			Valve.SnCode[3] = ModbusPara.rBuf[6];  			// ����ֵ
			Valve.SnCode[4] = ModbusPara.rBuf[7];  			// ����ֵ
            I2CPageWrite_Nbytes(ADDR_SN, LEN_SN, Valve.SnCode);
        }
        else if(func_num==9)
        {// ������ָ��
            if(ModbusPara.rBuf[3]==MY_MODBUS || ModbusPara.rBuf[3]==EXT_COMM)
            {
//                syspara.typeProtocal = ModbusPara.rBuf[3];
                prInfo(syspara.typeInfo, "\r\n set protocal to");
                if(ModbusPara.rBuf[3]==MY_MODBUS)
                    prInfo(syspara.typeInfo, "\r\n MODBUS");
                else
                    prInfo(syspara.typeInfo, "\r\n EXT PROTCAL");
                I2CPageWrite_Nbytes(ADDR_PROTOCAL, LEN_PROTOCAL, &ModbusPara.rBuf[3]);
            }
        }
        else if(func_num==0x63)
        {
    		if(port_num && port_num<=32)
    		{// д��ͨ����
                I2CPageWrite_Nbytes(ADDR_PORT_CNT, LEN_PORT_CNT, &port_num);
    		}
        }
        else if(func_num==0xff)
        {
            prInfo(syspara.typeInfo, "\r\n rcv para: %02x %02x %02x %02x",
                ModbusPara.rBuf[3], ModbusPara.rBuf[5], ModbusPara.rBuf[6], ModbusPara.rBuf[7]);
            I2CPageWrite_Nbytes(ADDR_MODULE_NUM, LEN_MODULE_NUM, &ModbusPara.rBuf[3]);
            I2CPageWrite_Nbytes(ADDR_VALVE_FIX, LEN_VALVE_FIX, &ModbusPara.rBuf[6]);
            I2CPageWrite_Nbytes(ADDR_DIR_FIX, LEN_DIR_FIX, &ModbusPara.rBuf[7]);
            I2CPageWrite_Nbytes(ADDR_PORT_CNT, LEN_PORT_CNT, &ModbusPara.rBuf[5]);
        }
        
		ModbusPara.tBuf[0] = ModbusPara.rBuf[0]; 			// �豸��ַ
		ModbusPara.tBuf[1] = ModbusPara.rBuf[1];  			// ������
		ModbusPara.tBuf[2] = ModbusPara.rBuf[2];  			// �˿ڱ��
		ModbusPara.tBuf[3] = ModbusPara.rBuf[3];  			// ����ֵ
		byteCount = 4;
		reg_num = ModbusCRC16( &ModbusPara.tBuf[0], byteCount);	// ��ȡCRC
		ModbusPara.tBuf[byteCount] = reg_num >> 8;
		byteCount++;
		ModbusPara.tBuf[byteCount] = reg_num ;
		byteCount++;
		if(ModbusPara.tBuf[0] != MB_Broadcast_ADDR)
           ModbusSend(byteCount);
        
        prInfo(syspara.comInfo, "\r\n s:");
        for(uint8 i=0; i<byteCount; i++)
        {
            prInfo(syspara.comInfo, " %02x", ModbusPara.tBuf[i]);
        }
	}
	else
	{// ����ϵͳ�涨���ּĴ���������
		ModbusPara.sERR = ERR_MB_ADDR;
	}
}



void MB_PresetMultipleHoldingRegisters(void)
{ // ������16
	unsigned short reg_num;
    unsigned char dvc_addr, func_num, port_num, byteCount;

	dvc_addr = ModbusPara.rBuf[0];		//ģ���ַ
	func_num = ModbusPara.rBuf[2];		//�˿ڱ��
	port_num = ModbusPara.rBuf[3];		//�˿ڱ��
	if(dvc_addr <= ADDR_MAX)
	{// ģ���ַ�ж�OK
	    if(func_num==0)
	    {
    		if(port_num && port_num<=valveFix.fix.portCnt)
    		{// ͨ������ж�OK,��ʼ��Ӧ����
                if(Valve.status==VALVE_RUN_END)
                	Valve.portDes = port_num;
                else if(Valve.status==VALVE_RUNNING)
                {
                    if(Valve.serialNum<PREPORTCNT)
                    {
                        Valve.serialPort[Valve.serialNum] = port_num;
                        Valve.serialNum++;
                    }
                }
            	Valve.spd = ModbusPara.rBuf[4];		//
            	Valve.dir = ModbusPara.rBuf[5];		//
            	if(!Valve.spd)
            	{
                    I2CPageRead_Nbytes(ADDR_SPD, LEN_SPD, &Valve.spd);
                    if(!Valve.spd || Valve.spd>SPD_LMT)
                        Valve.spd = SPD_VALVE;
                }
                speed[AXSV] = accel[AXSV] = decel[AXSV] = 100;
                
                speed[AXSV] *= (Valve.spd);
                speed[AXSV] *= (rdc.rate);
                accel[AXSV] *= (Valve.spd);
                accel[AXSV] *= (rdc.rate);
                decel[AXSV] *= (Valve.spd);
                decel[AXSV] *= (rdc.rate);
                
    			ModbusPara.tBuf[0] = ModbusPara.rBuf[0]; 			// �豸��ַ
    			ModbusPara.tBuf[1] = ModbusPara.rBuf[1];  			// ������
    			ModbusPara.tBuf[2] = ModbusPara.rBuf[2];  			// �˿ڱ��
    			ModbusPara.tBuf[3] = ModbusPara.rBuf[3];  			// �˿ڱ��
    			byteCount = 4;
    			reg_num = ModbusCRC16( &ModbusPara.tBuf[0], byteCount);	// ��ȡCRC
    			ModbusPara.tBuf[byteCount] = reg_num >> 8;
    			byteCount++;
    			ModbusPara.tBuf[byteCount] = reg_num ;
    			byteCount++;
    			if(ModbusPara.tBuf[0] != MB_Broadcast_ADDR)
                   ModbusSend(byteCount);
                
                prInfo(syspara.comInfo, "\r\n s:");
                for(uint8 i=0; i<byteCount; i++)
                {
                    prInfo(syspara.comInfo, " %02x", ModbusPara.tBuf[i]);
                }
    		}
    		else
    		{// ����ϵͳ�涨���ּĴ�����ַ��Χ���߶�ȡ��������ʼ��ַ��ƥ�����
    			ModbusPara.sERR = ERR_MB_DATA;
    		}
	    }
		else
		{// ����ϵͳ�涨���ּĴ�����ַ��Χ���߶�ȡ��������ʼ��ַ��ƥ�����
			ModbusPara.sERR = ERR_MB_DATA;
		}
	}
	else
	{// ����ϵͳ�涨���ּĴ���������
		ModbusPara.sERR = ERR_MB_ADDR;
	}
}



void ModbusProces(void)
{
//    uint32 i=0;
    if(ModbusPara.sRUN==MB_RECIVE_END)
	{
        if(ModbusPara.rCnt>LEAST_RCV_CNT)
    	{
            LED_WORK = !LED_WORK;
    		if(ModbusCRC16(&ModbusPara.rBuf[0], ModbusPara.rCnt)==0)
    		{
                uint32 i=0;
                prInfo(syspara.comInfo, "\r\n r:");
                for(i=0; i<ModbusPara.rCnt; i++)
                {
                    prInfo(syspara.comInfo, " %02x", ModbusPara.rBuf[i]);
//                    #ifdef INFO_DEBUG
//                    if(errRecord.bPrint==false)
//                        errRecord.cmdLast[i] = ModbusPara.rBuf[i];
//                    #endif
                }
                
    			{
                //ȷ��ģ����ڲ��ҹ�������
				switch(ModbusPara.rBuf[1])
				{
					case GET_HOLDING_REGT:
						MB_ReadHoldingRegisters();
						break;
					case PRESET_HOLDING_sREGT:
						MB_PresetSingleHoldingRegister();
						break;
					case PRESET_HOLDING_mREGT:
						MB_PresetMultipleHoldingRegisters();
						break;
					default:
						ModbusPara.sERR = ERR_MB_FUN;
						break;
    			}
    		}
    		}
    		else
    		{
    			ModbusPara.sERR = ERR_MB_DEVICE;
    		}
    		ModbusPara.rCnt = 0;
    		Modbus_ERROR();
    		ModbusPara.sRUN = MB_IDEL;
        }
        else
        {
    		ModbusPara.rCnt = 0;
    		Modbus_ERROR();
    		ModbusPara.sRUN = MB_IDEL;
        }
	}
}




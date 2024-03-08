/**
 * @file gps_dataconvert.c
 * @author JackeyZ
 * @brief (1) 功能:		用于处理GPS模块相关的数据
 * @version 0.1
 * @date 2024-01-19
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "includes.h"

/* GPS自定义变量 */
#define GPS_DATA_SIZE 100							// 定义GPS数据存储数组的长度

static volatile uint8_t GPS_Data[GPS_DATA_SIZE];	// 定义GPS数据存储数组

static volatile uint8_t GP_RMC[100];				// 定义GPRMC数组				
static volatile uint8_t GP_GGA[100];				// 定义GPGGA数组

static volatile uint8_t gps_point 	= 0;			// 定义GPS数据缓冲区提取point

volatile uint8_t  UTCTIME_AVAILABLE = 0;			// UTC定位准确标志
volatile uint8_t  RMCTIME_AVAILABLE = 0;			// RMC定位准确标志
volatile uint8_t  GPS_STAR_CNT = 0;				    // 卫星数量

GPS_TypeDef GPS;									// 定义GPS时间数据全局变量
GPS_pre_TypeDef GPS_pre_para;			            // 定义上一次GPS的关键参数
GPS_flags_TypeDef GPS_flags;			            // GPS_flags_TypeDef GPS_flags = {0,0,0,1,1,1,0};	

/**
 * @brief GPS结构体初始化
 * 
 */
static void GPS_FLAGS_INIT()
{
	GPS_flags.GPSpps_cnt_current = 0;
	GPS_flags.GPSpps_interval = 0;
	GPS_flags.GPSpps_flag = 0;
	GPS_flags.gps_convert_finish_flag = 1;
	GPS_flags.gga_over_flag = 1;
	GPS_flags.rmc_over_flag = 1;
	GPS_flags.gps_signal_count = 0;
}

/**
 * @brief GPS转换开始之前需要把标志位置0
 * 
 */
void GPS_Convert_Start()
{
	GPS_flags.gps_convert_finish_flag = 0;
	GPS_flags.rmc_over_flag = 0;
	GPS_flags.gga_over_flag = 0;
}

/**
 * @brief GGA数据包处理函数
 * 
 * @param s 传入需要解析的数组
 * @param len 传入数组的长度
 */
static void GPS_GGA(char* s,uint32_t len)
{
	char *begin;						
	uint32_t count;		

	uint8_t len_tmp = 1;
	uint32_t res=0;	
	char *p;
	
	begin=s;							
	count=0;						
	while((begin[count])!=','){count++;}		    // 寻找','
	if(count)
	{}												// 数据包帧头
	
	begin+=count+1;									// 跳过已读数据，继续读取后面的数据,这时begin指向的是GPGGA数据包的第二块数据的首地址
	count=0;
	while((begin[count])!=','){count++;}			// 寻找','
	if(count)										// 如果第一个逗号与第二个逗号之间存在数据
	{
		GPS.hh  = ( (*(begin + 0) - 0x30) * 10 ) + ( *(begin + 1) - 0x30 );		
		GPS.mm  = ( (*(begin + 2) - 0x30) * 10 ) + ( *(begin + 3) - 0x30 );
		GPS.ss  = ( (*(begin + 4) - 0x30) * 10 ) + ( *(begin + 5) - 0x30 );	
		GPS.sss = ( (*(begin + 7) - 0x30) * 10 ) + ( *(begin + 8) - 0x30 );		//sss实际为00，以整秒跳秒
		
		GPS_pre_para.UTCTIME_MS_pre = ((GPS.hh*60+GPS.mm)*60+GPS.ss)*1000+GPS.sss;
	}												// <1>定位UTC时间
	
	begin+=count+1;
	count=0;
	while((begin[count])!=','){count++;}			// 寻找','
	if(count)
	{}												// <2>纬度
		
	begin+=count+1;
	count=0;
	while((begin[count])!=','){count++;}			// 寻找','
	if(count)
	{}												// <3>北纬、南纬
		
	begin+=count+1;
	count=0;
	while((begin[count])!=','){count++;}			// 寻找','
	if(count)
	{}												// <4>经度
		
	begin+=count+1;
	count=0;
	while((begin[count])!=','){count++;}			// 寻找','
	if(count)
	{}												// <5>东经、西经
		
	begin+=count+1;
	count=0;
	while((begin[count])!=','){count++;}            // 寻找','
	if(count)
	{													
		if( ((*begin) == '1') || ((*begin) == '2') )// 1:非差分定位,2:差分定位
		{
			UTCTIME_AVAILABLE = SET;				// 定位成功
		}
		else
		{
			UTCTIME_AVAILABLE = RESET;			    // 定位失败20180917更改，0：未定位 3：无效PPS 6:正在估算
		}
	}
	else 
	{
		UTCTIME_AVAILABLE = RESET;					// 定位失败	
	}												// <6>定位状态

	begin+=count;
	count=0;
	
	while(begin[count+len_tmp] != ',')
	{
		len_tmp++;
	}
	//先提取星数字符串长度
	if(len_tmp>1)
	{
		len_tmp-=1;
		p = &begin[count+1];
		while(len_tmp--)
		{
			uint8_t tmp;
			tmp=*p++;
			if(tmp>='0' && tmp<='9')
				res=res*10+(tmp-'0');
		}
		GPS_STAR_CNT = res;
	}
	else
	{
		GPS_STAR_CNT = 0;
	}
	
	memset((void *)GP_GGA,'0',sizeof(GP_GGA));		// 清除GP_RMC数组中的内容
}


/**
 * @brief RMC数据包处理函数
 * 
 * @param s 传入需要解析的数组
 * @param len 传入数组的长度
 */
static void GPS_RMC(char* s,uint32_t len)
{
	char *begin;
	uint32_t count;
	
	begin=s;
	count=0;
	while((begin[count])!=',')
	{
		count++;
	}			//寻找','
	if(count)
	{}												// 帧头
	
	begin+=count+1; 								// 跳过已读数据，继续读取后面的数据
	count=0;
	while((begin[count])!=',')
	{
		count++;
	}			                                    // 寻找',',若为','，则count不加一，不执行后面的操作，只跳过','
	if(count)
	{
		GPS.hh  = ( (*(begin + 0) - 0x30) * 10 ) + ( *(begin + 1) - 0x30 );		
		GPS.mm  = ( (*(begin + 2) - 0x30) * 10 ) + ( *(begin + 3) - 0x30 );
		GPS.ss  = ( (*(begin + 4) - 0x30) * 10 ) + ( *(begin + 5) - 0x30 );	
		GPS.sss = ( (*(begin + 7) - 0x30) * 10 ) + ( *(begin + 8) - 0x30 );		// sss实际为00，以整秒跳秒
		
		GPS_pre_para.UTCTIME_MS_pre = ((GPS.hh*60+GPS.mm)*60+GPS.ss)*1000+GPS.sss;
	}												// <1>定位UTC时间
	
	begin+=count+1;
	count=0;
	while((begin[count])!=','){count++;}
	if(count)
	{	
		if( (*begin) == 'A' )
		{
			RMCTIME_AVAILABLE = SET;				// 定位有效
		}	
		else				
		{
			RMCTIME_AVAILABLE = RESET;				// 定位无效
		}
	}												// <2>定位状态，A有效，V无效
		
	begin+=count+1;
	count=0;
	while((begin[count])!=','){count++;}			// 寻找','
	if(count)
	{}												// <3>纬度
		
	begin+=count+1;
	count=0;
	while((begin[count])!=','){count++;}			// 寻找','
	if(count)
	{}												// <4>纬度半球：北或南
		
	begin+=count+1;
	count=0;
	while((begin[count])!=','){count++;}			// 寻找','
	if(count)
	{}												// <5>经度
		
	begin+=count+1;
	count=0;
	while((begin[count])!=','){count++;}			// 寻找','
	if(count)
	{}												// <6>经度半球：东或西
		
	begin+=count+1;
	count=0;
	while((begin[count])!=','){count++;}			// 寻找','
	if(count)
	{}												// <7>地面速率
		
	begin+=count+1;
	count=0;
	while((begin[count])!=','){count++;}			// 寻找','
	if(count)
	{}												// <8>地面航向
		
	begin+=count+1;
	count=0;
	while((begin[count])!=',')
	{
		count++;
	}			//寻找','
	if(count)
	{	
		GPS.day 	= ( (*(begin + 0) - 0x30) * 10 ) + ( *(begin + 1) - 0x30 );
		GPS.mon 	= ( (*(begin + 2) - 0x30) * 10 ) + ( *(begin + 3) - 0x30 );
		GPS.year	= ( (*(begin + 4) - 0x30) * 10 ) + ( *(begin + 5) - 0x30 );	
	}												// <9>UTC日期ddmmyy
	
	memset((void *)GP_RMC,'0',sizeof(GP_RMC));		// 清除GP_RMC数组中的内容
	
}

/**
 * @brief GPS的数据转换和数据更新（GGA和RMC的数据包）
 * 
 * @param data_length 传入的数组长度
 */
static void GPS_Data_Check_Convert_and_Update(uint16_t data_length)
{	
	uint16_t i = 0;

	if(GPS_Data[2] == 'G' && GPS_Data[3] == 'G' && GPS_Data[4] == 'A' && GPS_flags.gga_over_flag == 0)
	{					
		for(i=0; i<data_length;i++)
		{
			GP_GGA[i] = GPS_Data[i];
		}	
			
		GPS_GGA((char *)GP_GGA,data_length);			// 调用GGA时间处理函数
			
		GPS_flags.gga_over_flag = 1;
	}
	else if(GPS_Data[2] == 'R' && GPS_Data[3] == 'M' && GPS_Data[4] == 'C' && GPS_flags.rmc_over_flag == 0)
	{
		for(i=0; i<data_length;i++)
		{
			GP_RMC[i] = GPS_Data[i];
		}	
		GPS_RMC((char*)GP_RMC,data_length);				// 调用RMC时间处理函数	
		GPS_flags.rmc_over_flag = 1;
	}
	//首次解释完RMC和GGA数据则认为完成，不再接收新数据避免覆盖
	if((GPS_flags.rmc_over_flag ==1) && (GPS_flags.gga_over_flag == 1))
			GPS_flags.gps_convert_finish_flag = 1;	
}

/**
 * @brief GPS数据处理函数
 * 
 */
void GPSDataTask(void)
{
	volatile uint8_t temp;							// 临时变量,用于暂时保存数据
	
	/* 设置变量为静态局部变量,限制其他函数使用这些变量,但是函数执行完成以后不销毁,因为在下一次函数进来仍然需要使用 */	
	static volatile uint8_t 	Start_Save	= RESET;// 开始存储GPS数据
	static volatile uint8_t 	Save_Over 	= RESET;// GPS数据存储完成
	static volatile uint16_t 	counter,length;		// 计数器,存储长度
	static volatile uint8_t 	GPS_XOR,XOR_Flag;	// 校验和,校验标志
	static volatile uint8_t 	XOR_H,XOR_L;		// 校验和高低字节	
	
	/* 如果数据接收完成,则调用数据提取函数 */
	if(Save_Over)									// 判断数据是否接收完成
	{
		if(GPS_flags.gps_convert_finish_flag == 0)
		{	
			GPS_Data_Check_Convert_and_Update(length);	// 进行数据提取		
		}
		Save_Over = RESET;							// 复位存储完成标记
	}	
	
	/* main主循环中一直查询GPS串口缓冲区，如果数据缓冲区中有数据需要处理,则提取一个字节的数据进行处理 */
	if(gps_point != gps_index)						// 先进先出,选择一个字节进行处理
	{
		temp = GPS_Buf[gps_point];
		gps_point++;
		if(gps_point >= GPS_BUF_SIZE)				// 如果超出缓冲区大小,复位gps_point
		{
			gps_point = 0;
		}
	}
	else 											// 缓冲区没有新的数据需要处理,直接返回
	{
		return;
	}
	
	/* 如根据GPS的数据包特点进行数据的存储 */
	if( temp == '$' )		// 如果是'S'字符,则让数据存储到GPS_Data[]数组里面
	{
		counter 	= 0;	// 数据计数变量清零
		
		Start_Save 	= SET;	// 开始存储数据
		
		XOR_Flag 	= SET;	// 允许进行校验和运算

		GPS_XOR 	= 0;	// 复位校验和运算结果
		XOR_H 		= 0;	// 校验和计算结果高四位
		XOR_L 		= 0;	// 校验和计算结果低四位
	}
	else if(Start_Save)
	{		
		if( (temp == '\n') && (GPS_Data[counter-1] == '\r') ) // 如果数据是'\n',且上一位是'\r'
		{
			Start_Save = RESET;				// 复位开始存储标志位	
			
			XOR_H = (GPS_XOR & 0xF0) >> 4;	// 取校验和结果高四位
			XOR_L = (GPS_XOR & 0x0F);		// 取校验和结果低四位
			
			/* 将校验和结果转化为ASCII码 */
			if( XOR_H <= 9 )
			{
				XOR_H += 0x30;	
			}
			else if( (XOR_H >= 0x0a) && (XOR_H <= 0x0f) )
			{
				XOR_H = (XOR_H - 10) + 0x41;
			}
			else
			{
				return ;
			}
			
			if( XOR_L <= 9 )
			{
				XOR_L += 0x30;
			}
			else if( (XOR_L >= 0x0a) && (XOR_L <= 0x0f) )
			{
				XOR_L = (XOR_L - 10) + 0x41;
			}
			else
			{
				return ;
			}
							
			// 如果运算出的校验和与接收到的校验和进行一致,则更新"存储完成"标志位
			if( (XOR_H == GPS_Data[counter-3]) && (XOR_L == GPS_Data[counter-2]) )
			{
				length = counter;					// 将数据长度更新到length
				Save_Over = SET;					// 置位存储完成标志位
			}
		}
		else
		{			
			GPS_Data[counter] = temp;	// 将数据保存到数组
			
			if(temp == '*')				// 判断是否到了结束校验和的时候,GPS数据校验和是从$到*之间的数据进行异或运算
			{
				XOR_Flag = RESET;		// 禁止进行校验和运算
			}
			else if(XOR_Flag == SET)	// 如果允许校验和运算
			{
				GPS_XOR ^= temp;		// 进行校验和运算	
			}			
			counter++;
		}
	}
}

/**
 * @brief GPS秒脉冲外部中断配置函数（上升沿触发）
 * 
 */
static void GPS_EXTI_Config(void)
{
    GPIO_Config_T gpio_init_struct;
	EINT_Config_T eint_init_struct;

    RCM_EnableAHB1PeriphClock(RCM_AHB1_PERIPH_GPIOE);
    RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_SYSCFG);
											
	gpio_init_struct.pin = GPIO_PIN_13; 				    // PE13
	gpio_init_struct.mode = GPIO_MODE_IN;			     	// 输入模式
	gpio_init_struct.speed = GPIO_SPEED_50MHz;		        // 速度
	gpio_init_struct.pupd = GPIO_PUPD_DOWN;				    // 下拉
	GPIO_Config(GPIOE, &gpio_init_struct);

	EINT_ClearIntFlag(EINT_LINE_13);             		    // 清除中断标志位

	SYSCFG_ConfigEINTLine(SYSCFG_PORT_GPIOE,SYSCFG_PIN_13); // 中断线与IO口的映射关系配置

	eint_init_struct.line = EINT_LINE_13;					 // 中断线13
	eint_init_struct.mode = EINT_MODE_INTERRUPT;		     // 中断模式
	eint_init_struct.trigger = EINT_TRIGGER_RISING;		     // 上升沿触发
	eint_init_struct.lineCmd = ENABLE;					     // 使能中断线
	EINT_Config(&eint_init_struct);  	
	
	NVIC_EnableIRQRequest(EINT15_10_IRQn, 1, 0);						
}

/**
 * @brief 初始化GPS函数主要包括：
 * 		  1、初始化GPS标志位结构体
 * 		  2、初始化GPS的外部中断（GPIOE13通道）
 */
void InitGps(void)
{
	GPS_FLAGS_INIT();		// 初始化GPS的标志位
	
	GPS_EXTI_Config();		// 初始化GPS的秒脉冲中断
}

/**
 * @brief 将GPS的数据打包发送至PC端并刷新GPS关键参数
 * 
 */
void SendGPSData(void)
{
		USARTx_Send_Char(USART1,0x01);							// 帧头
		USARTx_Send_Char(USART1,0xFF);							// 帧头
		USARTx_Send_Char(USART1,0xFF);							// 帧头
		USARTx_Send_Char(USART1,0xFF);							// 帧头
		USARTx_Send_Char(USART1,0xFF);							// 帧头
		USARTx_Send_Char(USART1,0xFF);							// 帧头
	
	// 如果卫星数达到3,定位准确且信号质量好,则发送数据
	if(GPS_pre_para.RMCTIME_AVAILABLE_pre == SET && GPS_pre_para.UTCTIME_AVAILABLE_pre == SET)
	{		
		USARTx_Send_Char(USART1,0x0A);							           // 信号强度可以
		USARTx_Send_Char(USART1,(uint8_t)( time6_ms & 0x000000FF));		   // time_ms0-7
		USARTx_Send_Char(USART1,(uint8_t)((time6_ms & 0x0000FF00) >> 8));  // time_ms8-15
		USARTx_Send_Char(USART1,(uint8_t)((time6_ms & 0x00FF0000) >> 16)); // time_ms16-23
		USARTx_Send_Char(USART1,(uint8_t)((time6_ms & 0xFF000000) >> 24)); // time_ms24-31
		USARTx_Send_Char(USART1,GPS.year);							       // 年
		USARTx_Send_Char(USART1,GPS.mon);							       // 月
		USARTx_Send_Char(USART1,GPS.day);							       // 日
		USARTx_Send_Char(USART1,GPS_pre_para.GPS_STAR_CNT_pre);			   // 卫星数目	
	}
	else if( (time_s>TIME_INVALIDS)&&(RTCTime.data_valid_flag==1) )	       // 只要RTC数据有效都使用
	{
		//printf("Signal weak, send RTC time to PC\r\n");	
		USARTx_Send_Char(USART1,0x2A);							           // 信号强度可以
		USARTx_Send_Char(USART1,(uint8_t)( time6_ms & 0x000000FF));		   // r_time_ms0-7
		USARTx_Send_Char(USART1,(uint8_t)((time6_ms & 0x0000FF00) >> 8));  // r_time_ms8-15
		USARTx_Send_Char(USART1,(uint8_t)((time6_ms & 0x00FF0000) >> 16)); // r_time_ms16-23
		USARTx_Send_Char(USART1,(uint8_t)((time6_ms & 0xFF000000) >> 24)); // r_time_ms24-31
		USARTx_Send_Char(USART1,RTCTime.Year);							   // 年
		USARTx_Send_Char(USART1,RTCTime.Month);							   // 月
		USARTx_Send_Char(USART1,RTCTime.Day);							   // 日
		USARTx_Send_Char(USART1,0);					                       // 卫星数目	
	}
	else
	{
		//printf("GPS RTC not work\r\n");
		USARTx_Send_Char(USART1,0x0F);							//信号强度差
		USARTx_Send_Char(USART1,0x00);							//r_time_ms0-7
		USARTx_Send_Char(USART1,0x00);							//r_time_ms8-15
		USARTx_Send_Char(USART1,0x00);							//r_time_ms16-23
		USARTx_Send_Char(USART1,0x00);							//r_time_ms24-31
		USARTx_Send_Char(USART1,0x00);							//年
		USARTx_Send_Char(USART1,0x00);							//月
		USARTx_Send_Char(USART1,0x00);							//日
		USARTx_Send_Char(USART1,0x00);							//卫星数目
	}
	
	//刷新GPS关键参数
	GPS_pre_para.GPS_STAR_CNT_pre = GPS_STAR_CNT;               // GPS中提取判断是否有效（卫星数）
	
	GPS_pre_para.RMCTIME_AVAILABLE_pre = RMCTIME_AVAILABLE;     // GPS中提取判断是否有效（RMC定位精准）

	GPS_pre_para.UTCTIME_AVAILABLE_pre = UTCTIME_AVAILABLE;     // GPS中提取判断是否有效（GGA中UTC定位精准）
}

/**
 * @brief 发送GPS的数据函数
 * 
 * @note 如果标志位有效则发送
 */
void SendGpsTask(void)
{
	if(Time2SendGPSData == SET)						//如果允许发送
	{
		SendGPSData();								//发送数据
	
		Time2SendGPSData  = RESET;					//清空发送标记
	}  
}

/*************GPS外部中断函数***************/

/**
 * @brief GPS外部中断函数主要功能是：
 * 		  1、判断是否发送GPS脉冲中断
 * 		  2、如果GPS的脉冲中断间隔在设定范围内则证明有效允许更新RTC时间
 * 
 * @note GPIOE的13通道为GPS的1pps脉冲外部中断函数（0.5S一次脉冲）
 */
void EINT15_10_IRQHandler(void)                             // GPS外部中断（1pps）
{
	if(EINT_ReadIntFlag(EINT_LINE_13) == SET)			    // 判断是否发生了GPS秒脉冲中断
	{								
        if(GPS_flags.GPSpps_flag == 0)					    // 首次捕捉到GPS的秒脉冲中断
        {
            GPS_flags.GPSpps_flag = 1;
            GPS_flags.GPSpps_cnt_current = 0;
        }
        else												// 只有当相邻两个秒脉冲在设定范围内才认为有效，允许更新RTC
        {
            GPS_flags.GPSpps_interval = GPS_flags.GPSpps_cnt_current;
            GPS_flags.GPSpps_cnt_current = 0;
            if( (GPS_flags.GPSpps_interval >= GPS_INTERVAL_LOW) && (GPS_flags.GPSpps_interval <= GPS_INTERVAL_HIGH) )
            {
                Update_RTC_Time = SET;					    // 允许更新RTC时间
            }
        }	
		EINT_ClearIntFlag(EINT_LINE_13);					// 清除外部中断
	}
}
/**
 * @file ds3231s.c
 * @author JackeyZ
 * @brief (1) 功能:		用于处理RTC模块相关的数据
 * @version 0.1
 * @date 2023-12-28
 * 
 * @copyright Copyright (c) 2023 SEEKWAY
 * 
 */
#include "includes.h"

/* 1101 000x 这是器件的地址(从机地址)，最后一位x是读写选择位，为0时表示写操作，1表示读操作 */
#define DS3231_WriteAddress 0xD0    // 器件写地址 
#define DS3231_ReadAddress  0xD1    // 器件读地址 

/* 以下是DS3231内部的寄存器地址 */
#define DS3231_SECOND       0x00    // 秒
#define DS3231_MINUTE       0x01    // 分
#define DS3231_HOUR         0x02    // 时
#define DS3231_WEEK         0x03    // 星期
#define DS3231_DAY          0x04    // 日
#define DS3231_MONTH        0x05    // 月
#define DS3231_YEAR         0x06    // 年
/* 闹铃1 */            
#define DS3231_SALARM1ECOND 0x07    // 秒
#define DS3231_ALARM1MINUTE 0x08    // 分
#define DS3231_ALARM1HOUR   0x09    // 时
#define DS3231_ALARM1WEEK   0x0A    // 星期/日
/* 闹铃2 */
#define DS3231_ALARM2MINUTE 0x0b    // 分
#define DS3231_ALARM2HOUR   0x0c    // 时
#define DS3231_ALARM2WEEK   0x0d    // 星期/日

#define DS3231_CONTROL      0x0e    // 控制寄存器
#define DS3231_STATUS       0x0f    // 状态寄存器
#define BSY                 2       // 忙
#define OSF                 7       // 振荡器停止标志
#define DS3231_XTAL         0x10    // 晶体老化寄存器
#define DS3231_TEMPERATUREH 0x11    // 温度寄存器高字节(8位)
#define DS3231_TEMPERATUREL 0x12    // 温度寄存器低字节(高2位) 

/* DS3231S与GPS模块PPS的时间偏差 */
#define TIME_OFFSET  500
#define TIME_OFFSET2 503

/* 自定义变量区域 */
Rtc_Type RTCTime;				    // 用于存储当前的RTC时间日期
Rtc_Type curr_RTCTime;	            // 用于保存从RTC读取的时间日期
uint8_t rtc_calibrate_flag = 0;     // 表示RTC已被校准
uint8_t rtc_offset_flag = 0;	    // 0表示OFFSET补偿，1表示曾经复写rtc用TIME_OFFSET2补偿


/* *************  IIC-GPIO初始化  ************* */
/**
 * @brief IIC-GPIO初始化
 * 
 */
void IIC_GPIO_Config(void)
{
	GPIO_Config_T gpio_init_struct;
	
	RCM_EnableAHB1PeriphClock(RCM_AHB1_PERIPH_GPIOB);

	gpio_init_struct.pin = GPIO_PIN_8 | GPIO_PIN_9;	// PB8-SCL,PB9-SDA	
	gpio_init_struct.mode = GPIO_MODE_OUT;			// 输出模式  
	gpio_init_struct.otype = GPIO_OTYPE_OD;			// 开漏
	gpio_init_struct.pupd = GPIO_PUPD_UP;			// 上拉
	gpio_init_struct.speed = GPIO_SPEED_100MHz;		// 高速
	
	GPIO_Config(GPIOB, &gpio_init_struct);

	GPIO_SetBit(GPIOB, GPIO_PIN_8);				    // 初始化以后需要将GPIO设为自己希望得到的电平,这是一个好习惯
	
	GPIO_SetBit(GPIOB, GPIO_PIN_9);
}



/**
  * 函    数：I2C写SCL引脚电平
  * 参    数：BitValue 协议层传入的当前需要写入SCL的电平，范围0~1
  * 返 回 值：无
  * 注意事项：此函数需要用户实现内容，当BitValue为0时，需要置SCL为低电平，当BitValue为1时，需要置SCL为高电平
  */
void MyI2C_W_SCL(uint8_t BitValue)
{
	GPIO_WriteBitValue(GPIOB, GPIO_PIN_8, BitValue);		    // 根据BitValue，设置SCL引脚的电平
	delay_us(10);												// 延时10us，防止时序频率超过要求
}

/**
  * 函    数：I2C写SDA引脚电平
  * 参    数：BitValue 协议层传入的当前需要写入SDA的电平，范围0~0xFF
  * 返 回 值：无
  * 注意事项：此函数需要用户实现内容，当BitValue为0时，需要置SDA为低电平，当BitValue非0时，需要置SDA为高电平
  */
void MyI2C_W_SDA(uint8_t BitValue)
{
	GPIO_WriteBitValue(GPIOB, GPIO_PIN_9, BitValue);		    // 根据BitValue，设置SDA引脚的电平，BitValue要实现非0即1的特性
	delay_us(10);												// 延时10us，防止时序频率超过要求
}

/**
  * 函    数：I2C读SDA引脚电平
  * 参    数：无
  * 返 回 值：协议层需要得到的当前SDA的电平，范围0~1
  * 注意事项：此函数需要用户实现内容，当前SDA为低电平时，返回0，当前SDA为高电平时，返回1
  */
uint8_t MyI2C_R_SDA(void)
{
	uint8_t BitValue;
	BitValue = GPIO_ReadInputBit(GPIOB, GPIO_PIN_9);		    // 读取SDA电平
	delay_us(10);												// 延时10us，防止时序频率超过要求
	return BitValue;											// 返回SDA电平
}

/* *************  BCD码转换为HEX码  ************* */
/**
 * @brief BCD码转换为10进制
 * 
 * @param val 输入需要转换的BCD码
 * @return 转换后的HEX码
 */
static uint8_t BCD2HEX(uint8_t val)    	// BCD码的本质就是4个二进制数表示一个10进制数，4个位之间逢十进一
{										// 例如，十进制的12可以用HEX码0x0C表示，而用BCD码则是用0x12表示
	uint8_t temp = val & 0x0f;
	val = val >> 4;
	val %= 0xf;
	
	return val *10 + temp;
}

/* *************  HEX码转BCD码  ************* */
/**
 * @brief HEX码转BCD码
 * 
 * @param val 输入需要转换的BCD码
 * @return uint8_t 转换后的HEX码
 */
static uint8_t HEX2BCD(uint8_t val)		// HEX码转换为BCD码
{
    uint8_t i,j,k;
    i = val/10;							// 取Byte的十位
    j = val%10;							// 取Byte的个位						
    k = j+(i<<4);						// 十位左移4，末4位补0，再加上个位
    return k;
}

/*协议层*/
/**
  * 函    数：I2C起始
  * 参    数：无
  * 返 回 值：无
  */
void MyI2C_Start(void)
{
	MyI2C_W_SDA(1);					    // 释放SDA，确保SDA为高电平
	MyI2C_W_SCL(1);						// 释放SCL，确保SCL为高电平
	MyI2C_W_SDA(0);						// 在SCL高电平期间，拉低SDA，产生起始信号
	MyI2C_W_SCL(0);						// 起始后把SCL也拉低，即为了占用总线，也为了方便总线时序的拼接
}

/**
  * 函    数：I2C终止
  * 参    数：无
  * 返 回 值：无
  */
void MyI2C_Stop(void)
{
	MyI2C_W_SDA(0);						// 拉低SDA，确保SDA为低电平
	MyI2C_W_SCL(1);						// 释放SCL，使SCL呈现高电平
	MyI2C_W_SDA(1);						// 在SCL高电平期间，释放SDA，产生终止信号
}

/**
  * 函    数：I2C发送一个字节
  * 参    数：Byte 要发送的一个字节数据，范围：0x00~0xFF
  * 返 回 值：无
  */
void MyI2C_SendByte(uint8_t Byte)
{
	uint8_t i;
	for (i = 0; i < 8; i ++)			 // 循环8次，主机依次发送数据的每一位
	{
		MyI2C_W_SDA(Byte & (0x80 >> i));// 使用掩码的方式取出Byte的指定一位数据并写入到SDA线
		MyI2C_W_SCL(1);					// 释放SCL，从机在SCL高电平期间读取SDA
		MyI2C_W_SCL(0);					// 拉低SCL，主机开始发送下一位数据
	}
}

/**
  * 函    数：I2C接收一个字节
  * 参    数：无
  * 返 回 值：接收到的一个字节数据，范围：0x00~0xFF
  */
uint8_t MyI2C_ReceiveByte(void)
{
	uint8_t i, Byte = 0x00;			                    // 定义接收的数据，并赋初值0x00，此处必须赋初值0x00，后面会用到
	MyI2C_W_SDA(1);					                    // 接收前，主机先确保释放SDA，避免干扰从机的数据发送
	for (i = 0; i < 8; i ++)		                    // 循环8次，主机依次接收数据的每一位
	{
		MyI2C_W_SCL(1);				                    // 释放SCL，主机机在SCL高电平期间读取SDA
		if (MyI2C_R_SDA() == 1){Byte |= (0x80 >> i);}	// 读取SDA数据，并存储到Byte变量
														// 当SDA为1时，置变量指定位为1，当SDA为0时，不做处理，指定位为默认的初值0
		MyI2C_W_SCL(0);						            // 拉低SCL，从机在SCL低电平期间写入SDA
	}
	return Byte;							            // 返回接收到的一个字节数据
}

/**
  * 函    数：I2C发送应答位
  * 参    数：Byte 要发送的应答位，范围：0~1，0表示应答，1表示非应答
  * 返 回 值：无
  */
void MyI2C_SendAck(uint8_t AckBit)
{
	MyI2C_W_SDA(AckBit);					// 主机把应答位数据放到SDA线
	MyI2C_W_SCL(1);							// 释放SCL，从机在SCL高电平期间，读取应答位
	MyI2C_W_SCL(0);							// 拉低SCL，开始下一个时序模块
}

/**
  * 函    数：I2C接收应答位
  * 参    数：无
  * 返 回 值：接收到的应答位，范围：0~1，0表示应答，1表示非应答
  */
uint8_t MyI2C_ReceiveAck(void)
{
	uint8_t AckBit;							// 定义应答位变量
	MyI2C_W_SDA(1);							// 接收前，主机先确保释放SDA，避免干扰从机的数据发送
	MyI2C_W_SCL(1);							// 释放SCL，主机机在SCL高电平期间读取SDA
	AckBit = MyI2C_R_SDA();					// 将应答位存储到变量里
	MyI2C_W_SCL(0);							// 拉低SCL，开始下一个时序模块
	return AckBit;							// 返回定义应答位变量
}

/**
 * @brief I2C写数据
 * 
 * @param addr 写入器件的地址
 * @param write_data 需要写入的数据
 */
static void write_byte(uint8_t addr, uint8_t write_data)
{
	MyI2C_Start();						    // I2C起始
	MyI2C_SendByte(DS3231_WriteAddress);	// 发送从机地址，读写位为0，表示即将写入
	MyI2C_ReceiveAck();					    // 接收应答
	MyI2C_SendByte(addr);			        // 发送寄存器地址
	MyI2C_ReceiveAck();					    // 接收应答
	MyI2C_SendByte(write_data);				// 发送要写入寄存器的数据
	MyI2C_ReceiveAck();					    // 接收应答
	MyI2C_Stop();						    // I2C终止
}

/**
 * @brief 读出寄存器数据
 * 
 * @param random_addr 写入寄存器的地址
 * @return uint8_t 返回该寄存器的数据
 */
static uint8_t read_random(uint8_t random_addr)
{
	uint8_t Data;
	
	MyI2C_Start();						        // I2C起始
	MyI2C_SendByte(DS3231_WriteAddress);	    // 发送从机地址，读写位为0，表示即将写入
	MyI2C_ReceiveAck();					        // 接收应答
	MyI2C_SendByte(random_addr);			    // 发送寄存器地址
	MyI2C_ReceiveAck();					        // 接收应答
	
	MyI2C_Start();						        // I2C重复起始
	MyI2C_SendByte(DS3231_WriteAddress | 0x01);	// 发送从机地址，读写位为1，表示即将读取
	MyI2C_ReceiveAck();					        // 接收应答
	Data = MyI2C_ReceiveByte();			        // 接收指定寄存器的数据
	MyI2C_SendAck(1);					        // 发送应答，给从机非应答，终止从机的数据输出
	MyI2C_Stop();						        // I2C终止
	
	return Data;
}

/**
 * @brief 设置RTC的时间日期
 * 
 * @param yea 写入的年数据
 * @param mon 写入的月数据
 * @param da  写入的日数据
 * @param hou 写入的小时数据
 * @param min 写入的分钟数据
 * @param sec 写入的秒数据
 */
void ModifyTime(uint8_t yea,uint8_t mon,uint8_t da,uint8_t hou,uint8_t min,uint8_t sec)
{
    uint8_t temp=0;
   
    temp=HEX2BCD(yea);
    write_byte(DS3231_YEAR,temp);   //修改年
   
    temp=HEX2BCD(mon);
    write_byte(DS3231_MONTH,temp);  //修改月
   
    temp=HEX2BCD(da);
    write_byte(DS3231_DAY,temp);    //修改日
   
    temp=HEX2BCD(hou);
    write_byte(DS3231_HOUR,temp);   //修改时
   
    temp=HEX2BCD(min);
    write_byte(DS3231_MINUTE,temp); //修改分
   
    temp=HEX2BCD(sec);
    write_byte(DS3231_SECOND,temp); //修改秒
}

/**
 * @brief 获取RTC的时间
 * 
 */
void GetRTCTime()
{
	volatile uint8_t temp = 0;				                       //临时变量,用于临时存储读取RTC寄存器的返回值	

	memset(&RTCTime,0,sizeof(RTCTime));	                           //对结构体的成员全部初始化为0
	
	temp = read_random(DS3231_YEAR);
	RTCTime.Year = BCD2HEX(temp);
	
	temp = read_random(DS3231_MONTH);
	temp &= 0x1F;												    //不使用century
	RTCTime.Month = BCD2HEX(temp);
	
	temp = read_random(DS3231_DAY);
	RTCTime.Day = BCD2HEX(temp);

	temp = read_random(DS3231_HOUR);
	temp &= 0x3F;												    //使用24小时制
	RTCTime.Hour = BCD2HEX(temp);
	
	temp = read_random(DS3231_MINUTE);
	RTCTime.Min = BCD2HEX(temp);
	
	temp = read_random(DS3231_SECOND);
	RTCTime.Sec = BCD2HEX(temp);
	
	if(RTCTime.Sec==60)									            //正常运行程序，不会出现秒超过范围的情况(0-59)
		RTCTime.Sec = 0;
	
	if((RTCTime.Year==0)||(RTCTime.Month==0)||(RTCTime.Day==0))		//年月日任意一个为0，判定为RTC时间无效,BGR-N02
	{
		RTCTime.data_valid_flag = 0;
	}
	else
	{
		RTCTime.data_valid_flag = 1;
		if(rtc_offset_flag == 0)
			RTCTime.totalms = ((RTCTime.Hour*60+RTCTime.Min)*60+RTCTime.Sec)*1000+TIME_OFFSET;//计算当前RTC时间的毫秒数并更新到本地时间
		else
			RTCTime.totalms = ((RTCTime.Hour*60+RTCTime.Min)*60+RTCTime.Sec)*1000+TIME_OFFSET2;
		
		time6_ms = RTCTime.totalms;
	}
}

/**
 * @brief 比较从RTC读取的时间与写入的GPS的时间
 * 
 * @return uint8_t 0：时间不一致   1：时间一致
 */
uint8_t TimeCheck()
{
	volatile uint8_t temp = 0;					    //临时变量,用于临时存储读取RTC寄存器的返回值	

	memset(&curr_RTCTime,0,sizeof(curr_RTCTime));	//对结构体的成员全部初始化为0
	
	temp = read_random(DS3231_YEAR);
	curr_RTCTime.Year = BCD2HEX(temp);
	
	temp = read_random(DS3231_MONTH);
	temp &= 0x1F;								    //不使用century
	curr_RTCTime.Month = BCD2HEX(temp);
	
	temp = read_random(DS3231_DAY);
	curr_RTCTime.Day = BCD2HEX(temp);

	temp = read_random(DS3231_HOUR);
	temp &= 0x3F;									//使用24小时制
	curr_RTCTime.Hour = BCD2HEX(temp);
	
	temp = read_random(DS3231_MINUTE);
	curr_RTCTime.Min = BCD2HEX(temp);
	
	temp = read_random(DS3231_SECOND);
	curr_RTCTime.Sec = BCD2HEX(temp);

	if( (curr_RTCTime.Year == GPS.year) && (curr_RTCTime.Month == GPS.mon) && (curr_RTCTime.Day == GPS.day) && 
		(curr_RTCTime.Hour == GPS.hh) && (curr_RTCTime.Min == GPS.mm) && (curr_RTCTime.Sec == GPS.ss) )
		return 1;
	else
		return 0;
}

/**
 * @brief RTC方波输出中断配置函数
 * 
 */
static void RTC_EXTI_Config(void)
{
	GPIO_Config_T gpio_init_struct;						     // GPIO 定义
	EINT_Config_T eint_init_struct; 						 // 外部中断定义

	RCM_EnableAHB1PeriphClock(RCM_AHB1_PERIPH_GPIOE);
	RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_SYSCFG);							
					
	gpio_init_struct.pin = GPIO_PIN_0; 				         // PB0
	gpio_init_struct.mode = GPIO_MODE_IN;				     // 输入模式
	gpio_init_struct.speed = GPIO_SPEED_100MHz;		         // 速度
	gpio_init_struct.pupd = GPIO_PUPD_NOPULL;				 // 浮空
	GPIO_Config(GPIOE, &gpio_init_struct);
    

	EINT_ClearIntFlag(EINT_LINE_0);             		     // 清除中断标志位

	SYSCFG_ConfigEINTLine(SYSCFG_PORT_GPIOE,SYSCFG_PIN_0);   // 中断线与IO口的映射关系配置

	eint_init_struct.line = EINT_LINE_0;					 // 中断线0
	eint_init_struct.mode = EINT_MODE_INTERRUPT;		     // 中断模式
	eint_init_struct.trigger = EINT_TRIGGER_RISING;		     // 上升沿触发
	eint_init_struct.lineCmd = ENABLE;					     // 使能中断线
	EINT_Config(&eint_init_struct);  	

	NVIC_EnableIRQRequest(EINT0_IRQn, 0, 1);							
}

/**
 * @brief RTC配置函数内容包括：
 * 		  1、I2C总线初始化
 * 		  2、RTC的方波中断初始化
 * 		  3、启动RTC芯片的振荡器
 * 		  4、重置状态寄存器
 * 
 */
void InitRtc(void)
{		
	IIC_GPIO_Config();							// I2C总线初始化
	
	RTC_EXTI_Config();							// 初始化RTC的方波中断
	
	write_byte(DS3231_CONTROL,0x00);            // 启动振荡器,禁止电池方波输出,不转换温度,1Hz秒脉冲输出,不使用闹钟中断
	
	write_byte(DS3231_STATUS,0x00);             // 重置状态寄存器的值
}

/**
 * @brief 用GPS的时间更新RTC以及本地ms计数器的时间
 * 
 */
void UpdateTimeTask(void)
{
	if(Update_RTC_Time)							                              // 允许更新时间
	{
		GPS_Convert_Start();	                 				              // 上一轮数据已转换，允许新一轮数据获取
		if(GPS_pre_para.RMCTIME_AVAILABLE_pre==SET && GPS_pre_para.UTCTIME_AVAILABLE_pre == SET)
		{
			time_s = 0;							                              // 复位GPS-RTC秒计数器	
			time6_ms = GPS_pre_para.UTCTIME_MS_pre;				              // 信号强度足够的时候,更新	time6_ms时间以及关键参数		
			GPS_flags.gps_signal_count++;
			
			if(GPS_flags.gps_signal_count >= CALIBRATE_TIMES)
			{
				GPS_flags.gps_signal_count = CALIBRATE_TIMES;


				ModifyTime(GPS.year,GPS.mon,GPS.day,GPS.hh,GPS.mm,(GPS.ss)); // 连续获取3次有效的GPS信号则更新RTC时间
				if(TimeCheck() == 0)				                         // if RTC write time fail, rewrite RTC
				{
					ModifyTime(GPS.year,GPS.mon,GPS.day,GPS.hh,GPS.mm,(GPS.ss));
					rtc_offset_flag = 1;
				}
				else{rtc_offset_flag = 0;}
				
				if(rtc_calibrate_flag == 0)
				{
					rtc_calibrate_flag = 1;
				}
			}
		}
		else{
			GPS_flags.gps_signal_count = 0;
		}

		Update_RTC_Time = RESET;				                              //清除更新标志			
	}
}

/**
 * @brief 获取RTC的时间
 * 
 */
void LoadRtcTimeTask(void)					
{	
	if(GET_RTC_Time == SET)				        //RTC秒脉冲到来，会检查是否需要取RTC的时间数据									
	{
		if(time_s > TIME_INVALIDS)	            //GPS已经超过规定时间没有信号了
		{
			GPS_flags.gps_signal_count = 0;
			GetRTCTime();						//获取RTC时间
		}
		GET_RTC_Time = RESET;					//清除发送标志
	}
}

/*************RTC外部中断函数***************/

/**
 * @brief RTC外部中断函数
 * 
 */
void EINT0_IRQHandler(void)                                 // RTC外部中断
{
	if(EINT_ReadIntFlag(EINT_LINE_0) == SET)				// 判断是否发生了RTC方波中断
	{
		time_s++;											// 计算最近一次更新到现在的秒数
		
		GET_RTC_Time = SET;									// 允许获取RTC的时间

		EINT_ClearIntFlag(EINT_LINE_0);				        // 清除RTC中断
	}
}

/**
 * @file ds3231s.c
 * @author JackeyZ
 * @brief (1) ����:		���ڴ���RTCģ����ص�����
 * @version 0.1
 * @date 2023-12-28
 * 
 * @copyright Copyright (c) 2023 SEEKWAY
 * 
 */
#include "includes.h"

/* 1101 000x ���������ĵ�ַ(�ӻ���ַ)�����һλx�Ƕ�дѡ��λ��Ϊ0ʱ��ʾд������1��ʾ������ */
#define DS3231_WriteAddress 0xD0    // ����д��ַ 
#define DS3231_ReadAddress  0xD1    // ��������ַ 

/* ������DS3231�ڲ��ļĴ�����ַ */
#define DS3231_SECOND       0x00    // ��
#define DS3231_MINUTE       0x01    // ��
#define DS3231_HOUR         0x02    // ʱ
#define DS3231_WEEK         0x03    // ����
#define DS3231_DAY          0x04    // ��
#define DS3231_MONTH        0x05    // ��
#define DS3231_YEAR         0x06    // ��
/* ����1 */            
#define DS3231_SALARM1ECOND 0x07    // ��
#define DS3231_ALARM1MINUTE 0x08    // ��
#define DS3231_ALARM1HOUR   0x09    // ʱ
#define DS3231_ALARM1WEEK   0x0A    // ����/��
/* ����2 */
#define DS3231_ALARM2MINUTE 0x0b    // ��
#define DS3231_ALARM2HOUR   0x0c    // ʱ
#define DS3231_ALARM2WEEK   0x0d    // ����/��

#define DS3231_CONTROL      0x0e    // ���ƼĴ���
#define DS3231_STATUS       0x0f    // ״̬�Ĵ���
#define BSY                 2       // æ
#define OSF                 7       // ����ֹͣ��־
#define DS3231_XTAL         0x10    // �����ϻ��Ĵ���
#define DS3231_TEMPERATUREH 0x11    // �¶ȼĴ������ֽ�(8λ)
#define DS3231_TEMPERATUREL 0x12    // �¶ȼĴ������ֽ�(��2λ) 

/* DS3231S��GPSģ��PPS��ʱ��ƫ�� */
#define TIME_OFFSET  500
#define TIME_OFFSET2 503

/* �Զ���������� */
Rtc_Type RTCTime;				    // ���ڴ洢��ǰ��RTCʱ������
Rtc_Type curr_RTCTime;	            // ���ڱ����RTC��ȡ��ʱ������
uint8_t rtc_calibrate_flag = 0;     // ��ʾRTC�ѱ�У׼
uint8_t rtc_offset_flag = 0;	    // 0��ʾOFFSET������1��ʾ������дrtc��TIME_OFFSET2����


/* *************  IIC-GPIO��ʼ��  ************* */
/**
 * @brief IIC-GPIO��ʼ��
 * 
 */
void IIC_GPIO_Config(void)
{
	GPIO_Config_T gpio_init_struct;
	
	RCM_EnableAHB1PeriphClock(RCM_AHB1_PERIPH_GPIOB);

	gpio_init_struct.pin = GPIO_PIN_8 | GPIO_PIN_9;	// PB8-SCL,PB9-SDA	
	gpio_init_struct.mode = GPIO_MODE_OUT;			// ���ģʽ  
	gpio_init_struct.otype = GPIO_OTYPE_OD;			// ��©
	gpio_init_struct.pupd = GPIO_PUPD_UP;			// ����
	gpio_init_struct.speed = GPIO_SPEED_100MHz;		// ����
	
	GPIO_Config(GPIOB, &gpio_init_struct);

	GPIO_SetBit(GPIOB, GPIO_PIN_8);				    // ��ʼ���Ժ���Ҫ��GPIO��Ϊ�Լ�ϣ���õ��ĵ�ƽ,����һ����ϰ��
	
	GPIO_SetBit(GPIOB, GPIO_PIN_9);
}



/**
  * ��    ����I2CдSCL���ŵ�ƽ
  * ��    ����BitValue Э��㴫��ĵ�ǰ��Ҫд��SCL�ĵ�ƽ����Χ0~1
  * �� �� ֵ����
  * ע������˺�����Ҫ�û�ʵ�����ݣ���BitValueΪ0ʱ����Ҫ��SCLΪ�͵�ƽ����BitValueΪ1ʱ����Ҫ��SCLΪ�ߵ�ƽ
  */
void MyI2C_W_SCL(uint8_t BitValue)
{
	GPIO_WriteBitValue(GPIOB, GPIO_PIN_8, BitValue);		    // ����BitValue������SCL���ŵĵ�ƽ
	delay_us(10);												// ��ʱ10us����ֹʱ��Ƶ�ʳ���Ҫ��
}

/**
  * ��    ����I2CдSDA���ŵ�ƽ
  * ��    ����BitValue Э��㴫��ĵ�ǰ��Ҫд��SDA�ĵ�ƽ����Χ0~0xFF
  * �� �� ֵ����
  * ע������˺�����Ҫ�û�ʵ�����ݣ���BitValueΪ0ʱ����Ҫ��SDAΪ�͵�ƽ����BitValue��0ʱ����Ҫ��SDAΪ�ߵ�ƽ
  */
void MyI2C_W_SDA(uint8_t BitValue)
{
	GPIO_WriteBitValue(GPIOB, GPIO_PIN_9, BitValue);		    // ����BitValue������SDA���ŵĵ�ƽ��BitValueҪʵ�ַ�0��1������
	delay_us(10);												// ��ʱ10us����ֹʱ��Ƶ�ʳ���Ҫ��
}

/**
  * ��    ����I2C��SDA���ŵ�ƽ
  * ��    ������
  * �� �� ֵ��Э�����Ҫ�õ��ĵ�ǰSDA�ĵ�ƽ����Χ0~1
  * ע������˺�����Ҫ�û�ʵ�����ݣ���ǰSDAΪ�͵�ƽʱ������0����ǰSDAΪ�ߵ�ƽʱ������1
  */
uint8_t MyI2C_R_SDA(void)
{
	uint8_t BitValue;
	BitValue = GPIO_ReadInputBit(GPIOB, GPIO_PIN_9);		    // ��ȡSDA��ƽ
	delay_us(10);												// ��ʱ10us����ֹʱ��Ƶ�ʳ���Ҫ��
	return BitValue;											// ����SDA��ƽ
}

/* *************  BCD��ת��ΪHEX��  ************* */
/**
 * @brief BCD��ת��Ϊ10����
 * 
 * @param val ������Ҫת����BCD��
 * @return ת�����HEX��
 */
static uint8_t BCD2HEX(uint8_t val)    	// BCD��ı��ʾ���4������������ʾһ��10��������4��λ֮���ʮ��һ
{										// ���磬ʮ���Ƶ�12������HEX��0x0C��ʾ������BCD��������0x12��ʾ
	uint8_t temp = val & 0x0f;
	val = val >> 4;
	val %= 0xf;
	
	return val *10 + temp;
}

/* *************  HEX��תBCD��  ************* */
/**
 * @brief HEX��תBCD��
 * 
 * @param val ������Ҫת����BCD��
 * @return uint8_t ת�����HEX��
 */
static uint8_t HEX2BCD(uint8_t val)		// HEX��ת��ΪBCD��
{
    uint8_t i,j,k;
    i = val/10;							// ȡByte��ʮλ
    j = val%10;							// ȡByte�ĸ�λ						
    k = j+(i<<4);						// ʮλ����4��ĩ4λ��0���ټ��ϸ�λ
    return k;
}

/*Э���*/
/**
  * ��    ����I2C��ʼ
  * ��    ������
  * �� �� ֵ����
  */
void MyI2C_Start(void)
{
	MyI2C_W_SDA(1);					    // �ͷ�SDA��ȷ��SDAΪ�ߵ�ƽ
	MyI2C_W_SCL(1);						// �ͷ�SCL��ȷ��SCLΪ�ߵ�ƽ
	MyI2C_W_SDA(0);						// ��SCL�ߵ�ƽ�ڼ䣬����SDA��������ʼ�ź�
	MyI2C_W_SCL(0);						// ��ʼ���SCLҲ���ͣ���Ϊ��ռ�����ߣ�ҲΪ�˷�������ʱ���ƴ��
}

/**
  * ��    ����I2C��ֹ
  * ��    ������
  * �� �� ֵ����
  */
void MyI2C_Stop(void)
{
	MyI2C_W_SDA(0);						// ����SDA��ȷ��SDAΪ�͵�ƽ
	MyI2C_W_SCL(1);						// �ͷ�SCL��ʹSCL���ָߵ�ƽ
	MyI2C_W_SDA(1);						// ��SCL�ߵ�ƽ�ڼ䣬�ͷ�SDA��������ֹ�ź�
}

/**
  * ��    ����I2C����һ���ֽ�
  * ��    ����Byte Ҫ���͵�һ���ֽ����ݣ���Χ��0x00~0xFF
  * �� �� ֵ����
  */
void MyI2C_SendByte(uint8_t Byte)
{
	uint8_t i;
	for (i = 0; i < 8; i ++)			 // ѭ��8�Σ��������η������ݵ�ÿһλ
	{
		MyI2C_W_SDA(Byte & (0x80 >> i));// ʹ������ķ�ʽȡ��Byte��ָ��һλ���ݲ�д�뵽SDA��
		MyI2C_W_SCL(1);					// �ͷ�SCL���ӻ���SCL�ߵ�ƽ�ڼ��ȡSDA
		MyI2C_W_SCL(0);					// ����SCL��������ʼ������һλ����
	}
}

/**
  * ��    ����I2C����һ���ֽ�
  * ��    ������
  * �� �� ֵ�����յ���һ���ֽ����ݣ���Χ��0x00~0xFF
  */
uint8_t MyI2C_ReceiveByte(void)
{
	uint8_t i, Byte = 0x00;			                    // ������յ����ݣ�������ֵ0x00���˴����븳��ֵ0x00��������õ�
	MyI2C_W_SDA(1);					                    // ����ǰ��������ȷ���ͷ�SDA��������Ŵӻ������ݷ���
	for (i = 0; i < 8; i ++)		                    // ѭ��8�Σ��������ν������ݵ�ÿһλ
	{
		MyI2C_W_SCL(1);				                    // �ͷ�SCL����������SCL�ߵ�ƽ�ڼ��ȡSDA
		if (MyI2C_R_SDA() == 1){Byte |= (0x80 >> i);}	// ��ȡSDA���ݣ����洢��Byte����
														// ��SDAΪ1ʱ���ñ���ָ��λΪ1����SDAΪ0ʱ����������ָ��λΪĬ�ϵĳ�ֵ0
		MyI2C_W_SCL(0);						            // ����SCL���ӻ���SCL�͵�ƽ�ڼ�д��SDA
	}
	return Byte;							            // ���ؽ��յ���һ���ֽ�����
}

/**
  * ��    ����I2C����Ӧ��λ
  * ��    ����Byte Ҫ���͵�Ӧ��λ����Χ��0~1��0��ʾӦ��1��ʾ��Ӧ��
  * �� �� ֵ����
  */
void MyI2C_SendAck(uint8_t AckBit)
{
	MyI2C_W_SDA(AckBit);					// ������Ӧ��λ���ݷŵ�SDA��
	MyI2C_W_SCL(1);							// �ͷ�SCL���ӻ���SCL�ߵ�ƽ�ڼ䣬��ȡӦ��λ
	MyI2C_W_SCL(0);							// ����SCL����ʼ��һ��ʱ��ģ��
}

/**
  * ��    ����I2C����Ӧ��λ
  * ��    ������
  * �� �� ֵ�����յ���Ӧ��λ����Χ��0~1��0��ʾӦ��1��ʾ��Ӧ��
  */
uint8_t MyI2C_ReceiveAck(void)
{
	uint8_t AckBit;							// ����Ӧ��λ����
	MyI2C_W_SDA(1);							// ����ǰ��������ȷ���ͷ�SDA��������Ŵӻ������ݷ���
	MyI2C_W_SCL(1);							// �ͷ�SCL����������SCL�ߵ�ƽ�ڼ��ȡSDA
	AckBit = MyI2C_R_SDA();					// ��Ӧ��λ�洢��������
	MyI2C_W_SCL(0);							// ����SCL����ʼ��һ��ʱ��ģ��
	return AckBit;							// ���ض���Ӧ��λ����
}

/**
 * @brief I2Cд����
 * 
 * @param addr д�������ĵ�ַ
 * @param write_data ��Ҫд�������
 */
static void write_byte(uint8_t addr, uint8_t write_data)
{
	MyI2C_Start();						    // I2C��ʼ
	MyI2C_SendByte(DS3231_WriteAddress);	// ���ʹӻ���ַ����дλΪ0����ʾ����д��
	MyI2C_ReceiveAck();					    // ����Ӧ��
	MyI2C_SendByte(addr);			        // ���ͼĴ�����ַ
	MyI2C_ReceiveAck();					    // ����Ӧ��
	MyI2C_SendByte(write_data);				// ����Ҫд��Ĵ���������
	MyI2C_ReceiveAck();					    // ����Ӧ��
	MyI2C_Stop();						    // I2C��ֹ
}

/**
 * @brief �����Ĵ�������
 * 
 * @param random_addr д��Ĵ����ĵ�ַ
 * @return uint8_t ���ظüĴ���������
 */
static uint8_t read_random(uint8_t random_addr)
{
	uint8_t Data;
	
	MyI2C_Start();						        // I2C��ʼ
	MyI2C_SendByte(DS3231_WriteAddress);	    // ���ʹӻ���ַ����дλΪ0����ʾ����д��
	MyI2C_ReceiveAck();					        // ����Ӧ��
	MyI2C_SendByte(random_addr);			    // ���ͼĴ�����ַ
	MyI2C_ReceiveAck();					        // ����Ӧ��
	
	MyI2C_Start();						        // I2C�ظ���ʼ
	MyI2C_SendByte(DS3231_WriteAddress | 0x01);	// ���ʹӻ���ַ����дλΪ1����ʾ������ȡ
	MyI2C_ReceiveAck();					        // ����Ӧ��
	Data = MyI2C_ReceiveByte();			        // ����ָ���Ĵ���������
	MyI2C_SendAck(1);					        // ����Ӧ�𣬸��ӻ���Ӧ����ֹ�ӻ����������
	MyI2C_Stop();						        // I2C��ֹ
	
	return Data;
}

/**
 * @brief ����RTC��ʱ������
 * 
 * @param yea д���������
 * @param mon д���������
 * @param da  д���������
 * @param hou д���Сʱ����
 * @param min д��ķ�������
 * @param sec д���������
 */
void ModifyTime(uint8_t yea,uint8_t mon,uint8_t da,uint8_t hou,uint8_t min,uint8_t sec)
{
    uint8_t temp=0;
   
    temp=HEX2BCD(yea);
    write_byte(DS3231_YEAR,temp);   //�޸���
   
    temp=HEX2BCD(mon);
    write_byte(DS3231_MONTH,temp);  //�޸���
   
    temp=HEX2BCD(da);
    write_byte(DS3231_DAY,temp);    //�޸���
   
    temp=HEX2BCD(hou);
    write_byte(DS3231_HOUR,temp);   //�޸�ʱ
   
    temp=HEX2BCD(min);
    write_byte(DS3231_MINUTE,temp); //�޸ķ�
   
    temp=HEX2BCD(sec);
    write_byte(DS3231_SECOND,temp); //�޸���
}

/**
 * @brief ��ȡRTC��ʱ��
 * 
 */
void GetRTCTime()
{
	volatile uint8_t temp = 0;				                       //��ʱ����,������ʱ�洢��ȡRTC�Ĵ����ķ���ֵ	

	memset(&RTCTime,0,sizeof(RTCTime));	                           //�Խṹ��ĳ�Աȫ����ʼ��Ϊ0
	
	temp = read_random(DS3231_YEAR);
	RTCTime.Year = BCD2HEX(temp);
	
	temp = read_random(DS3231_MONTH);
	temp &= 0x1F;												    //��ʹ��century
	RTCTime.Month = BCD2HEX(temp);
	
	temp = read_random(DS3231_DAY);
	RTCTime.Day = BCD2HEX(temp);

	temp = read_random(DS3231_HOUR);
	temp &= 0x3F;												    //ʹ��24Сʱ��
	RTCTime.Hour = BCD2HEX(temp);
	
	temp = read_random(DS3231_MINUTE);
	RTCTime.Min = BCD2HEX(temp);
	
	temp = read_random(DS3231_SECOND);
	RTCTime.Sec = BCD2HEX(temp);
	
	if(RTCTime.Sec==60)									            //�������г��򣬲�������볬����Χ�����(0-59)
		RTCTime.Sec = 0;
	
	if((RTCTime.Year==0)||(RTCTime.Month==0)||(RTCTime.Day==0))		//����������һ��Ϊ0���ж�ΪRTCʱ����Ч,BGR-N02
	{
		RTCTime.data_valid_flag = 0;
	}
	else
	{
		RTCTime.data_valid_flag = 1;
		if(rtc_offset_flag == 0)
			RTCTime.totalms = ((RTCTime.Hour*60+RTCTime.Min)*60+RTCTime.Sec)*1000+TIME_OFFSET;//���㵱ǰRTCʱ��ĺ����������µ�����ʱ��
		else
			RTCTime.totalms = ((RTCTime.Hour*60+RTCTime.Min)*60+RTCTime.Sec)*1000+TIME_OFFSET2;
		
		time6_ms = RTCTime.totalms;
	}
}

/**
 * @brief �Ƚϴ�RTC��ȡ��ʱ����д���GPS��ʱ��
 * 
 * @return uint8_t 0��ʱ�䲻һ��   1��ʱ��һ��
 */
uint8_t TimeCheck()
{
	volatile uint8_t temp = 0;					    //��ʱ����,������ʱ�洢��ȡRTC�Ĵ����ķ���ֵ	

	memset(&curr_RTCTime,0,sizeof(curr_RTCTime));	//�Խṹ��ĳ�Աȫ����ʼ��Ϊ0
	
	temp = read_random(DS3231_YEAR);
	curr_RTCTime.Year = BCD2HEX(temp);
	
	temp = read_random(DS3231_MONTH);
	temp &= 0x1F;								    //��ʹ��century
	curr_RTCTime.Month = BCD2HEX(temp);
	
	temp = read_random(DS3231_DAY);
	curr_RTCTime.Day = BCD2HEX(temp);

	temp = read_random(DS3231_HOUR);
	temp &= 0x3F;									//ʹ��24Сʱ��
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
 * @brief RTC��������ж����ú���
 * 
 */
static void RTC_EXTI_Config(void)
{
	GPIO_Config_T gpio_init_struct;						     // GPIO ����
	EINT_Config_T eint_init_struct; 						 // �ⲿ�ж϶���

	RCM_EnableAHB1PeriphClock(RCM_AHB1_PERIPH_GPIOE);
	RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_SYSCFG);							
					
	gpio_init_struct.pin = GPIO_PIN_0; 				         // PB0
	gpio_init_struct.mode = GPIO_MODE_IN;				     // ����ģʽ
	gpio_init_struct.speed = GPIO_SPEED_100MHz;		         // �ٶ�
	gpio_init_struct.pupd = GPIO_PUPD_NOPULL;				 // ����
	GPIO_Config(GPIOE, &gpio_init_struct);
    

	EINT_ClearIntFlag(EINT_LINE_0);             		     // ����жϱ�־λ

	SYSCFG_ConfigEINTLine(SYSCFG_PORT_GPIOE,SYSCFG_PIN_0);   // �ж�����IO�ڵ�ӳ���ϵ����

	eint_init_struct.line = EINT_LINE_0;					 // �ж���0
	eint_init_struct.mode = EINT_MODE_INTERRUPT;		     // �ж�ģʽ
	eint_init_struct.trigger = EINT_TRIGGER_RISING;		     // �����ش���
	eint_init_struct.lineCmd = ENABLE;					     // ʹ���ж���
	EINT_Config(&eint_init_struct);  	

	NVIC_EnableIRQRequest(EINT0_IRQn, 0, 1);							
}

/**
 * @brief RTC���ú������ݰ�����
 * 		  1��I2C���߳�ʼ��
 * 		  2��RTC�ķ����жϳ�ʼ��
 * 		  3������RTCоƬ������
 * 		  4������״̬�Ĵ���
 * 
 */
void InitRtc(void)
{		
	IIC_GPIO_Config();							// I2C���߳�ʼ��
	
	RTC_EXTI_Config();							// ��ʼ��RTC�ķ����ж�
	
	write_byte(DS3231_CONTROL,0x00);            // ��������,��ֹ��ط������,��ת���¶�,1Hz���������,��ʹ�������ж�
	
	write_byte(DS3231_STATUS,0x00);             // ����״̬�Ĵ�����ֵ
}

/**
 * @brief ��GPS��ʱ�����RTC�Լ�����ms��������ʱ��
 * 
 */
void UpdateTimeTask(void)
{
	if(Update_RTC_Time)							                              // �������ʱ��
	{
		GPS_Convert_Start();	                 				              // ��һ��������ת����������һ�����ݻ�ȡ
		if(GPS_pre_para.RMCTIME_AVAILABLE_pre==SET && GPS_pre_para.UTCTIME_AVAILABLE_pre == SET)
		{
			time_s = 0;							                              // ��λGPS-RTC�������	
			time6_ms = GPS_pre_para.UTCTIME_MS_pre;				              // �ź�ǿ���㹻��ʱ��,����	time6_msʱ���Լ��ؼ�����		
			GPS_flags.gps_signal_count++;
			
			if(GPS_flags.gps_signal_count >= CALIBRATE_TIMES)
			{
				GPS_flags.gps_signal_count = CALIBRATE_TIMES;


				ModifyTime(GPS.year,GPS.mon,GPS.day,GPS.hh,GPS.mm,(GPS.ss)); // ������ȡ3����Ч��GPS�ź������RTCʱ��
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

		Update_RTC_Time = RESET;				                              //������±�־			
	}
}

/**
 * @brief ��ȡRTC��ʱ��
 * 
 */
void LoadRtcTimeTask(void)					
{	
	if(GET_RTC_Time == SET)				        //RTC�����嵽���������Ƿ���ҪȡRTC��ʱ������									
	{
		if(time_s > TIME_INVALIDS)	            //GPS�Ѿ������涨ʱ��û���ź���
		{
			GPS_flags.gps_signal_count = 0;
			GetRTCTime();						//��ȡRTCʱ��
		}
		GET_RTC_Time = RESET;					//������ͱ�־
	}
}

/*************RTC�ⲿ�жϺ���***************/

/**
 * @brief RTC�ⲿ�жϺ���
 * 
 */
void EINT0_IRQHandler(void)                                 // RTC�ⲿ�ж�
{
	if(EINT_ReadIntFlag(EINT_LINE_0) == SET)				// �ж��Ƿ�����RTC�����ж�
	{
		time_s++;											// �������һ�θ��µ����ڵ�����
		
		GET_RTC_Time = SET;									// �����ȡRTC��ʱ��

		EINT_ClearIntFlag(EINT_LINE_0);				        // ���RTC�ж�
	}
}

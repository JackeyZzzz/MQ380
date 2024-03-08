/**
 * @file gps_dataconvert.c
 * @author JackeyZ
 * @brief (1) ����:		���ڴ���GPSģ����ص�����
 * @version 0.1
 * @date 2024-01-19
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "includes.h"

/* GPS�Զ������ */
#define GPS_DATA_SIZE 100							// ����GPS���ݴ洢����ĳ���

static volatile uint8_t GPS_Data[GPS_DATA_SIZE];	// ����GPS���ݴ洢����

static volatile uint8_t GP_RMC[100];				// ����GPRMC����				
static volatile uint8_t GP_GGA[100];				// ����GPGGA����

static volatile uint8_t gps_point 	= 0;			// ����GPS���ݻ�������ȡpoint

volatile uint8_t  UTCTIME_AVAILABLE = 0;			// UTC��λ׼ȷ��־
volatile uint8_t  RMCTIME_AVAILABLE = 0;			// RMC��λ׼ȷ��־
volatile uint8_t  GPS_STAR_CNT = 0;				    // ��������

GPS_TypeDef GPS;									// ����GPSʱ������ȫ�ֱ���
GPS_pre_TypeDef GPS_pre_para;			            // ������һ��GPS�Ĺؼ�����
GPS_flags_TypeDef GPS_flags;			            // GPS_flags_TypeDef GPS_flags = {0,0,0,1,1,1,0};	

/**
 * @brief GPS�ṹ���ʼ��
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
 * @brief GPSת����ʼ֮ǰ��Ҫ�ѱ�־λ��0
 * 
 */
void GPS_Convert_Start()
{
	GPS_flags.gps_convert_finish_flag = 0;
	GPS_flags.rmc_over_flag = 0;
	GPS_flags.gga_over_flag = 0;
}

/**
 * @brief GGA���ݰ�������
 * 
 * @param s ������Ҫ����������
 * @param len ��������ĳ���
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
	while((begin[count])!=','){count++;}		    // Ѱ��','
	if(count)
	{}												// ���ݰ�֡ͷ
	
	begin+=count+1;									// �����Ѷ����ݣ�������ȡ���������,��ʱbeginָ�����GPGGA���ݰ��ĵڶ������ݵ��׵�ַ
	count=0;
	while((begin[count])!=','){count++;}			// Ѱ��','
	if(count)										// �����һ��������ڶ�������֮���������
	{
		GPS.hh  = ( (*(begin + 0) - 0x30) * 10 ) + ( *(begin + 1) - 0x30 );		
		GPS.mm  = ( (*(begin + 2) - 0x30) * 10 ) + ( *(begin + 3) - 0x30 );
		GPS.ss  = ( (*(begin + 4) - 0x30) * 10 ) + ( *(begin + 5) - 0x30 );	
		GPS.sss = ( (*(begin + 7) - 0x30) * 10 ) + ( *(begin + 8) - 0x30 );		//sssʵ��Ϊ00������������
		
		GPS_pre_para.UTCTIME_MS_pre = ((GPS.hh*60+GPS.mm)*60+GPS.ss)*1000+GPS.sss;
	}												// <1>��λUTCʱ��
	
	begin+=count+1;
	count=0;
	while((begin[count])!=','){count++;}			// Ѱ��','
	if(count)
	{}												// <2>γ��
		
	begin+=count+1;
	count=0;
	while((begin[count])!=','){count++;}			// Ѱ��','
	if(count)
	{}												// <3>��γ����γ
		
	begin+=count+1;
	count=0;
	while((begin[count])!=','){count++;}			// Ѱ��','
	if(count)
	{}												// <4>����
		
	begin+=count+1;
	count=0;
	while((begin[count])!=','){count++;}			// Ѱ��','
	if(count)
	{}												// <5>����������
		
	begin+=count+1;
	count=0;
	while((begin[count])!=','){count++;}            // Ѱ��','
	if(count)
	{													
		if( ((*begin) == '1') || ((*begin) == '2') )// 1:�ǲ�ֶ�λ,2:��ֶ�λ
		{
			UTCTIME_AVAILABLE = SET;				// ��λ�ɹ�
		}
		else
		{
			UTCTIME_AVAILABLE = RESET;			    // ��λʧ��20180917���ģ�0��δ��λ 3����ЧPPS 6:���ڹ���
		}
	}
	else 
	{
		UTCTIME_AVAILABLE = RESET;					// ��λʧ��	
	}												// <6>��λ״̬

	begin+=count;
	count=0;
	
	while(begin[count+len_tmp] != ',')
	{
		len_tmp++;
	}
	//����ȡ�����ַ�������
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
	
	memset((void *)GP_GGA,'0',sizeof(GP_GGA));		// ���GP_RMC�����е�����
}


/**
 * @brief RMC���ݰ�������
 * 
 * @param s ������Ҫ����������
 * @param len ��������ĳ���
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
	}			//Ѱ��','
	if(count)
	{}												// ֡ͷ
	
	begin+=count+1; 								// �����Ѷ����ݣ�������ȡ���������
	count=0;
	while((begin[count])!=',')
	{
		count++;
	}			                                    // Ѱ��',',��Ϊ','����count����һ����ִ�к���Ĳ�����ֻ����','
	if(count)
	{
		GPS.hh  = ( (*(begin + 0) - 0x30) * 10 ) + ( *(begin + 1) - 0x30 );		
		GPS.mm  = ( (*(begin + 2) - 0x30) * 10 ) + ( *(begin + 3) - 0x30 );
		GPS.ss  = ( (*(begin + 4) - 0x30) * 10 ) + ( *(begin + 5) - 0x30 );	
		GPS.sss = ( (*(begin + 7) - 0x30) * 10 ) + ( *(begin + 8) - 0x30 );		// sssʵ��Ϊ00������������
		
		GPS_pre_para.UTCTIME_MS_pre = ((GPS.hh*60+GPS.mm)*60+GPS.ss)*1000+GPS.sss;
	}												// <1>��λUTCʱ��
	
	begin+=count+1;
	count=0;
	while((begin[count])!=','){count++;}
	if(count)
	{	
		if( (*begin) == 'A' )
		{
			RMCTIME_AVAILABLE = SET;				// ��λ��Ч
		}	
		else				
		{
			RMCTIME_AVAILABLE = RESET;				// ��λ��Ч
		}
	}												// <2>��λ״̬��A��Ч��V��Ч
		
	begin+=count+1;
	count=0;
	while((begin[count])!=','){count++;}			// Ѱ��','
	if(count)
	{}												// <3>γ��
		
	begin+=count+1;
	count=0;
	while((begin[count])!=','){count++;}			// Ѱ��','
	if(count)
	{}												// <4>γ�Ȱ��򣺱�����
		
	begin+=count+1;
	count=0;
	while((begin[count])!=','){count++;}			// Ѱ��','
	if(count)
	{}												// <5>����
		
	begin+=count+1;
	count=0;
	while((begin[count])!=','){count++;}			// Ѱ��','
	if(count)
	{}												// <6>���Ȱ��򣺶�����
		
	begin+=count+1;
	count=0;
	while((begin[count])!=','){count++;}			// Ѱ��','
	if(count)
	{}												// <7>��������
		
	begin+=count+1;
	count=0;
	while((begin[count])!=','){count++;}			// Ѱ��','
	if(count)
	{}												// <8>���溽��
		
	begin+=count+1;
	count=0;
	while((begin[count])!=',')
	{
		count++;
	}			//Ѱ��','
	if(count)
	{	
		GPS.day 	= ( (*(begin + 0) - 0x30) * 10 ) + ( *(begin + 1) - 0x30 );
		GPS.mon 	= ( (*(begin + 2) - 0x30) * 10 ) + ( *(begin + 3) - 0x30 );
		GPS.year	= ( (*(begin + 4) - 0x30) * 10 ) + ( *(begin + 5) - 0x30 );	
	}												// <9>UTC����ddmmyy
	
	memset((void *)GP_RMC,'0',sizeof(GP_RMC));		// ���GP_RMC�����е�����
	
}

/**
 * @brief GPS������ת�������ݸ��£�GGA��RMC�����ݰ���
 * 
 * @param data_length ��������鳤��
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
			
		GPS_GGA((char *)GP_GGA,data_length);			// ����GGAʱ�䴦����
			
		GPS_flags.gga_over_flag = 1;
	}
	else if(GPS_Data[2] == 'R' && GPS_Data[3] == 'M' && GPS_Data[4] == 'C' && GPS_flags.rmc_over_flag == 0)
	{
		for(i=0; i<data_length;i++)
		{
			GP_RMC[i] = GPS_Data[i];
		}	
		GPS_RMC((char*)GP_RMC,data_length);				// ����RMCʱ�䴦����	
		GPS_flags.rmc_over_flag = 1;
	}
	//�״ν�����RMC��GGA��������Ϊ��ɣ����ٽ��������ݱ��⸲��
	if((GPS_flags.rmc_over_flag ==1) && (GPS_flags.gga_over_flag == 1))
			GPS_flags.gps_convert_finish_flag = 1;	
}

/**
 * @brief GPS���ݴ�����
 * 
 */
void GPSDataTask(void)
{
	volatile uint8_t temp;							// ��ʱ����,������ʱ��������
	
	/* ���ñ���Ϊ��̬�ֲ�����,������������ʹ����Щ����,���Ǻ���ִ������Ժ�����,��Ϊ����һ�κ���������Ȼ��Ҫʹ�� */	
	static volatile uint8_t 	Start_Save	= RESET;// ��ʼ�洢GPS����
	static volatile uint8_t 	Save_Over 	= RESET;// GPS���ݴ洢���
	static volatile uint16_t 	counter,length;		// ������,�洢����
	static volatile uint8_t 	GPS_XOR,XOR_Flag;	// У���,У���־
	static volatile uint8_t 	XOR_H,XOR_L;		// У��͸ߵ��ֽ�	
	
	/* ������ݽ������,�����������ȡ���� */
	if(Save_Over)									// �ж������Ƿ�������
	{
		if(GPS_flags.gps_convert_finish_flag == 0)
		{	
			GPS_Data_Check_Convert_and_Update(length);	// ����������ȡ		
		}
		Save_Over = RESET;							// ��λ�洢��ɱ��
	}	
	
	/* main��ѭ����һֱ��ѯGPS���ڻ�������������ݻ���������������Ҫ����,����ȡһ���ֽڵ����ݽ��д��� */
	if(gps_point != gps_index)						// �Ƚ��ȳ�,ѡ��һ���ֽڽ��д���
	{
		temp = GPS_Buf[gps_point];
		gps_point++;
		if(gps_point >= GPS_BUF_SIZE)				// ���������������С,��λgps_point
		{
			gps_point = 0;
		}
	}
	else 											// ������û���µ�������Ҫ����,ֱ�ӷ���
	{
		return;
	}
	
	/* �����GPS�����ݰ��ص�������ݵĴ洢 */
	if( temp == '$' )		// �����'S'�ַ�,�������ݴ洢��GPS_Data[]��������
	{
		counter 	= 0;	// ���ݼ�����������
		
		Start_Save 	= SET;	// ��ʼ�洢����
		
		XOR_Flag 	= SET;	// �������У�������

		GPS_XOR 	= 0;	// ��λУ���������
		XOR_H 		= 0;	// У��ͼ���������λ
		XOR_L 		= 0;	// У��ͼ���������λ
	}
	else if(Start_Save)
	{		
		if( (temp == '\n') && (GPS_Data[counter-1] == '\r') ) // ���������'\n',����һλ��'\r'
		{
			Start_Save = RESET;				// ��λ��ʼ�洢��־λ	
			
			XOR_H = (GPS_XOR & 0xF0) >> 4;	// ȡУ��ͽ������λ
			XOR_L = (GPS_XOR & 0x0F);		// ȡУ��ͽ������λ
			
			/* ��У��ͽ��ת��ΪASCII�� */
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
							
			// ����������У�������յ���У��ͽ���һ��,�����"�洢���"��־λ
			if( (XOR_H == GPS_Data[counter-3]) && (XOR_L == GPS_Data[counter-2]) )
			{
				length = counter;					// �����ݳ��ȸ��µ�length
				Save_Over = SET;					// ��λ�洢��ɱ�־λ
			}
		}
		else
		{			
			GPS_Data[counter] = temp;	// �����ݱ��浽����
			
			if(temp == '*')				// �ж��Ƿ��˽���У��͵�ʱ��,GPS����У����Ǵ�$��*֮������ݽ����������
			{
				XOR_Flag = RESET;		// ��ֹ����У�������
			}
			else if(XOR_Flag == SET)	// �������У�������
			{
				GPS_XOR ^= temp;		// ����У�������	
			}			
			counter++;
		}
	}
}

/**
 * @brief GPS�������ⲿ�ж����ú����������ش�����
 * 
 */
static void GPS_EXTI_Config(void)
{
    GPIO_Config_T gpio_init_struct;
	EINT_Config_T eint_init_struct;

    RCM_EnableAHB1PeriphClock(RCM_AHB1_PERIPH_GPIOE);
    RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_SYSCFG);
											
	gpio_init_struct.pin = GPIO_PIN_13; 				    // PE13
	gpio_init_struct.mode = GPIO_MODE_IN;			     	// ����ģʽ
	gpio_init_struct.speed = GPIO_SPEED_50MHz;		        // �ٶ�
	gpio_init_struct.pupd = GPIO_PUPD_DOWN;				    // ����
	GPIO_Config(GPIOE, &gpio_init_struct);

	EINT_ClearIntFlag(EINT_LINE_13);             		    // ����жϱ�־λ

	SYSCFG_ConfigEINTLine(SYSCFG_PORT_GPIOE,SYSCFG_PIN_13); // �ж�����IO�ڵ�ӳ���ϵ����

	eint_init_struct.line = EINT_LINE_13;					 // �ж���13
	eint_init_struct.mode = EINT_MODE_INTERRUPT;		     // �ж�ģʽ
	eint_init_struct.trigger = EINT_TRIGGER_RISING;		     // �����ش���
	eint_init_struct.lineCmd = ENABLE;					     // ʹ���ж���
	EINT_Config(&eint_init_struct);  	
	
	NVIC_EnableIRQRequest(EINT15_10_IRQn, 1, 0);						
}

/**
 * @brief ��ʼ��GPS������Ҫ������
 * 		  1����ʼ��GPS��־λ�ṹ��
 * 		  2����ʼ��GPS���ⲿ�жϣ�GPIOE13ͨ����
 */
void InitGps(void)
{
	GPS_FLAGS_INIT();		// ��ʼ��GPS�ı�־λ
	
	GPS_EXTI_Config();		// ��ʼ��GPS���������ж�
}

/**
 * @brief ��GPS�����ݴ��������PC�˲�ˢ��GPS�ؼ�����
 * 
 */
void SendGPSData(void)
{
		USARTx_Send_Char(USART1,0x01);							// ֡ͷ
		USARTx_Send_Char(USART1,0xFF);							// ֡ͷ
		USARTx_Send_Char(USART1,0xFF);							// ֡ͷ
		USARTx_Send_Char(USART1,0xFF);							// ֡ͷ
		USARTx_Send_Char(USART1,0xFF);							// ֡ͷ
		USARTx_Send_Char(USART1,0xFF);							// ֡ͷ
	
	// ����������ﵽ3,��λ׼ȷ���ź�������,��������
	if(GPS_pre_para.RMCTIME_AVAILABLE_pre == SET && GPS_pre_para.UTCTIME_AVAILABLE_pre == SET)
	{		
		USARTx_Send_Char(USART1,0x0A);							           // �ź�ǿ�ȿ���
		USARTx_Send_Char(USART1,(uint8_t)( time6_ms & 0x000000FF));		   // time_ms0-7
		USARTx_Send_Char(USART1,(uint8_t)((time6_ms & 0x0000FF00) >> 8));  // time_ms8-15
		USARTx_Send_Char(USART1,(uint8_t)((time6_ms & 0x00FF0000) >> 16)); // time_ms16-23
		USARTx_Send_Char(USART1,(uint8_t)((time6_ms & 0xFF000000) >> 24)); // time_ms24-31
		USARTx_Send_Char(USART1,GPS.year);							       // ��
		USARTx_Send_Char(USART1,GPS.mon);							       // ��
		USARTx_Send_Char(USART1,GPS.day);							       // ��
		USARTx_Send_Char(USART1,GPS_pre_para.GPS_STAR_CNT_pre);			   // ������Ŀ	
	}
	else if( (time_s>TIME_INVALIDS)&&(RTCTime.data_valid_flag==1) )	       // ֻҪRTC������Ч��ʹ��
	{
		//printf("Signal weak, send RTC time to PC\r\n");	
		USARTx_Send_Char(USART1,0x2A);							           // �ź�ǿ�ȿ���
		USARTx_Send_Char(USART1,(uint8_t)( time6_ms & 0x000000FF));		   // r_time_ms0-7
		USARTx_Send_Char(USART1,(uint8_t)((time6_ms & 0x0000FF00) >> 8));  // r_time_ms8-15
		USARTx_Send_Char(USART1,(uint8_t)((time6_ms & 0x00FF0000) >> 16)); // r_time_ms16-23
		USARTx_Send_Char(USART1,(uint8_t)((time6_ms & 0xFF000000) >> 24)); // r_time_ms24-31
		USARTx_Send_Char(USART1,RTCTime.Year);							   // ��
		USARTx_Send_Char(USART1,RTCTime.Month);							   // ��
		USARTx_Send_Char(USART1,RTCTime.Day);							   // ��
		USARTx_Send_Char(USART1,0);					                       // ������Ŀ	
	}
	else
	{
		//printf("GPS RTC not work\r\n");
		USARTx_Send_Char(USART1,0x0F);							//�ź�ǿ�Ȳ�
		USARTx_Send_Char(USART1,0x00);							//r_time_ms0-7
		USARTx_Send_Char(USART1,0x00);							//r_time_ms8-15
		USARTx_Send_Char(USART1,0x00);							//r_time_ms16-23
		USARTx_Send_Char(USART1,0x00);							//r_time_ms24-31
		USARTx_Send_Char(USART1,0x00);							//��
		USARTx_Send_Char(USART1,0x00);							//��
		USARTx_Send_Char(USART1,0x00);							//��
		USARTx_Send_Char(USART1,0x00);							//������Ŀ
	}
	
	//ˢ��GPS�ؼ�����
	GPS_pre_para.GPS_STAR_CNT_pre = GPS_STAR_CNT;               // GPS����ȡ�ж��Ƿ���Ч����������
	
	GPS_pre_para.RMCTIME_AVAILABLE_pre = RMCTIME_AVAILABLE;     // GPS����ȡ�ж��Ƿ���Ч��RMC��λ��׼��

	GPS_pre_para.UTCTIME_AVAILABLE_pre = UTCTIME_AVAILABLE;     // GPS����ȡ�ж��Ƿ���Ч��GGA��UTC��λ��׼��
}

/**
 * @brief ����GPS�����ݺ���
 * 
 * @note �����־λ��Ч����
 */
void SendGpsTask(void)
{
	if(Time2SendGPSData == SET)						//���������
	{
		SendGPSData();								//��������
	
		Time2SendGPSData  = RESET;					//��շ��ͱ��
	}  
}

/*************GPS�ⲿ�жϺ���***************/

/**
 * @brief GPS�ⲿ�жϺ�����Ҫ�����ǣ�
 * 		  1���ж��Ƿ���GPS�����ж�
 * 		  2�����GPS�������жϼ�����趨��Χ����֤����Ч�������RTCʱ��
 * 
 * @note GPIOE��13ͨ��ΪGPS��1pps�����ⲿ�жϺ�����0.5Sһ�����壩
 */
void EINT15_10_IRQHandler(void)                             // GPS�ⲿ�жϣ�1pps��
{
	if(EINT_ReadIntFlag(EINT_LINE_13) == SET)			    // �ж��Ƿ�����GPS�������ж�
	{								
        if(GPS_flags.GPSpps_flag == 0)					    // �״β�׽��GPS���������ж�
        {
            GPS_flags.GPSpps_flag = 1;
            GPS_flags.GPSpps_cnt_current = 0;
        }
        else												// ֻ�е������������������趨��Χ�ڲ���Ϊ��Ч���������RTC
        {
            GPS_flags.GPSpps_interval = GPS_flags.GPSpps_cnt_current;
            GPS_flags.GPSpps_cnt_current = 0;
            if( (GPS_flags.GPSpps_interval >= GPS_INTERVAL_LOW) && (GPS_flags.GPSpps_interval <= GPS_INTERVAL_HIGH) )
            {
                Update_RTC_Time = SET;					    // �������RTCʱ��
            }
        }	
		EINT_ClearIntFlag(EINT_LINE_13);					// ����ⲿ�ж�
	}
}
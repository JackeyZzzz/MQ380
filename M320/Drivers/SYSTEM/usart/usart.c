/**
 * @file usart.c
 * @author JackeyZ
 * @brief (1) 功能:		用于处理串口模块相关的数据
 * @version 0.1
 * @date 2023-12-28
 * 
 * @copyright Copyright (c) 2023 SEEKWAY
 * 
 */
#include "includes.h"

/* 串口2中断服务子程序，用于接收GPS数据 */	
volatile uint8_t Time2SendGPSData = RESET;					// 发送GPS数据到PC
volatile uint8_t gps_index = 0;								// GPS缓冲区“计数器”
volatile uint8_t GPS_Buf[GPS_BUF_SIZE] = {0};				// GPS缓冲区
uint8_t g_serialRxPacket[100];				                // 定义接收数据包数组，数据包格式"@MSG\r\n"


/* 串口接收缓冲区 */
uint8_t g_usart_rx_buf[USART_REC_LEN];

/* 串口接收状态
 * bit15  : 接收完成标志
 * bit14  : 接收到0x0D
 * bit13~0: 接收到的有效字节数
 */
uint16_t g_usart_rx_sta = 0;

/* 串口接收临时缓冲 */
uint8_t g_rx_buffer[RXBUFFERSIZE];

/**
 * @brief 所有用到的串口函数
 * 
 * @note 串口一波特率为115200 串口二波特率为9600
 */
void InitUsarts(void)
{ 
    USARTx_Init(USART1,115200); // 初始化串口1
    USARTx_Init(UART4,9600);    // 初始化串口2
}

/**
 * @brief 串口初始化
 * 
 * @param USARTx 设置串口通道
 * @param baudrate 设置波特率
 */
void USARTx_Init(USART_T *USARTx,uint32_t baudrate)
{
    GPIO_Config_T gpio_init_struct;
    USART_Config_T usart_init_struct;
    
    if(USARTx == USART1)
    {
        /* 使能时钟 */
        RCM_EnableAHB1PeriphClock(RCM_AHB1_PERIPH_GPIOA);
        RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_USART1);

        GPIO_ConfigPinAF(GPIOA, GPIO_PIN_SOURCE_9, GPIO_AF_USART1);
        GPIO_ConfigPinAF(GPIOA, GPIO_PIN_SOURCE_10, GPIO_AF_USART1);

        gpio_init_struct.pin = GPIO_PIN_9;
        gpio_init_struct.mode = GPIO_MODE_AF;
        gpio_init_struct.speed = GPIO_SPEED_100MHz;
        gpio_init_struct.otype = GPIO_OTYPE_PP;
        gpio_init_struct.pupd = GPIO_PUPD_NOPULL;
        GPIO_Config(GPIOA, &gpio_init_struct);
    
        gpio_init_struct.pin = GPIO_PIN_10;
        gpio_init_struct.mode = GPIO_MODE_AF;
        gpio_init_struct.speed = GPIO_SPEED_100MHz;
        gpio_init_struct.pupd = GPIO_PUPD_UP;
        gpio_init_struct.otype = GPIO_OTYPE_OD;
        GPIO_Config(GPIOA,&gpio_init_struct);

        NVIC_EnableIRQRequest(USART1_IRQn, 1, 0);
    }
    
    if(USARTx == UART4)
    {
        /* 使能时钟 */
        RCM_EnableAHB1PeriphClock(RCM_AHB1_PERIPH_GPIOA);
        RCM_EnableAPB1PeriphClock(RCM_APB1_PERIPH_UART4);

        GPIO_ConfigPinAF(GPIOA, GPIO_PIN_SOURCE_0, GPIO_AF_UART4);
        GPIO_ConfigPinAF(GPIOA, GPIO_PIN_SOURCE_1, GPIO_AF_UART4);

        gpio_init_struct.pin = GPIO_PIN_0;
        gpio_init_struct.mode = GPIO_MODE_AF;
        gpio_init_struct.speed = GPIO_SPEED_100MHz;
        gpio_init_struct.otype = GPIO_OTYPE_PP;
        gpio_init_struct.pupd = GPIO_PUPD_NOPULL;
        GPIO_Config(GPIOA, &gpio_init_struct);
    

        gpio_init_struct.pin = GPIO_PIN_1;
        gpio_init_struct.mode = GPIO_MODE_AF;
        gpio_init_struct.speed = GPIO_SPEED_100MHz;
        gpio_init_struct.otype = GPIO_OTYPE_PP;
        gpio_init_struct.pupd = GPIO_PUPD_UP;
        GPIO_Config(GPIOA,&gpio_init_struct);

        NVIC_EnableIRQRequest(UART4_IRQn, 3, 0);
    }
   
    usart_init_struct.baudRate = baudrate;                      /* 通讯波特率 */
    usart_init_struct.wordLength = USART_WORD_LEN_8B;           /* 数据位 */
    usart_init_struct.stopBits = USART_STOP_BIT_1;              /* 停止位 */
    usart_init_struct.parity = USART_PARITY_NONE;               /* 校验位 */
    usart_init_struct.mode = USART_MODE_TX_RX;                  /* 收发模式 */
    usart_init_struct.hardwareFlow = USART_HARDWARE_FLOW_NONE;  /* 无硬件流控 */

    USART_Config(USARTx, &usart_init_struct);
    USART_Enable(USARTx);

    USART_EnableInterrupt(USARTx, USART_INT_RXBNE);
    USART_ClearStatusFlag(USARTx, USART_FLAG_RXBNE);
}

/**
 * @brief USARTx send one byte
 * @param USART_TypeDef* 设置发送的通道
 * @param u8 ch 需要发送的数据
 * 
 */
void USARTx_Send_Char(USART_T* USARTx,u8 ch)
{
	USART_TxData(USARTx,ch);//发送数据
	while(!USART_ReadStatusFlag(USARTx, USART_FLAG_TXBE));//判断一个数据帧是否发送完成
}

/**
 * @brief 串口x发送字符串
 * @param USART_TypeDef* USARTx
 * @param u8 ch
 * 
 */
void USARTx_Send_String(USART_T* USARTx,char *str)
{
	while(*str!='\0')//判断字符串是否发送完成
	{
		USART_TxData(USARTx,*str);//发送数据
		while(!USART_ReadStatusFlag(USARTx, USART_FLAG_TXBE));//判断一个数据帧是否发送完成
		str++;//指针偏移
	}
}

/*
函数功能：串口x发送指定长度数据
参数：USART_TypeDef* USARTx,u8 *data_buff,u16 data_len
返回值：无
*/
void USARTx_Send_Buff(USART_T* USARTx,u8 *data_buff,u16 data_len)
{
    for(u16 i=0;i<data_len;i++)
    {
        USART_TxData(USARTx,*(data_buff+i));//发送数据
        while(!USART_ReadStatusFlag(USARTx, USART_FLAG_TXBE));//判断一个数据帧是否发送完成
    }
}



/*************串口中断函数***************/

/**
 * @brief 串口1中断服务函数 主要用于调试
 * 
 */
void USART1_IRQHandler(void)                                 // 串口一调试用
{
    if(USART_ReadStatusFlag(USART1,USART_FLAG_OVRE) != RESET)
    {
        USART_ClearStatusFlag(USART1,USART_FLAG_OVRE);
        USART_RxData(USART1);
    }
    if(USART_ReadStatusFlag(USART1,USART_FLAG_RXBNE) == SET)
    {
        USART_ClearIntFlag(USART1,USART_INT_RXBNE);
    }  
}

/**
 * @brief       串口4中断服务函数
 * @note        用于接收GPS模块发送的数据 
 *              GPS_Buf：GPS接收数据缓冲区 
 *              gps_index：GPS接收数据长度
 * @param       无
 * @retval      无
 */

void UART4_IRQHandler(void)                                  // GPS串口使用
{
	if(USART_ReadStatusFlag(UART4,USART_FLAG_OVRE) != RESET)
	{
		USART_ClearStatusFlag(UART4,USART_FLAG_OVRE);
		USART_RxData(UART4);
	}
	if(USART_ReadStatusFlag(UART4,USART_FLAG_RXBNE) == SET)
	{
		USART_ClearIntFlag(UART4,USART_INT_RXBNE);
		GPS_Buf[gps_index] = USART_RxData(UART4);
		gps_index++;
		if(gps_index>=GPS_BUF_SIZE)
		{
			gps_index = 0;
		}
	}  
}

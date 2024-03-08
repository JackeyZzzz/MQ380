/**
 * @file rs485.c
 * @author JackeyZ
 * @brief (1) 功能:		用于处理485模块相关的数据
 * @version 0.1
 * @date 2023-12-28
 * 
 * @copyright Copyright (c) 2023 SEEKWAY
 * 
 */
#include "includes.h"

/* 串口接收完成标志位 */	
uint8_t uart2RevFinish;
/* 数据发送区 */
uint8_t DmxDataPack[DMA_ALLDATA_LEN];

/* RS485接收缓存区 */ 	
uint8_t RS485_RX_BUF[64] = {0};  	                        // 接收缓冲,最大64个字节.
/* 接收到的数据长度 */
uint8_t RS485_RX_CNT=0;   	

/**********************************************初始化层***********************************************************/

/**
 * @brief 初始化IO 串口6 
 *        pclk1:PCLK1时钟频率(Mhz)
 *        bound:波特率	
 * 
 * @param USARTx 选择串口通道
 * @param baudrate 设置波特率
 */
void InitRs485(USART_T *USARTx,u32 baudrate)
{  
    GPIO_Config_T gpio_init_struct;
    USART_Config_T usart_init_struct;

    if(USARTx == UART5)
    {
         /* 使能时钟 */
        RCM_EnableAHB1PeriphClock(RCM_AHB1_PERIPH_GPIOC);
        RCM_EnableAHB1PeriphClock(RCM_AHB1_PERIPH_GPIOD);
        RCM_EnableAPB1PeriphClock(RCM_APB1_PERIPH_UART5);

        GPIO_ConfigPinAF(GPIOC, GPIO_PIN_SOURCE_12, GPIO_AF_UART5);
        GPIO_ConfigPinAF(GPIOD, GPIO_PIN_SOURCE_2, GPIO_AF_UART5);

        gpio_init_struct.pin = GPIO_PIN_12;
        gpio_init_struct.mode = GPIO_MODE_AF;
        gpio_init_struct.speed = GPIO_SPEED_100MHz;
        gpio_init_struct.otype = GPIO_OTYPE_PP;
        gpio_init_struct.pupd = GPIO_PUPD_NOPULL;
        GPIO_Config(GPIOC, &gpio_init_struct);

        gpio_init_struct.pin = GPIO_PIN_2;
        gpio_init_struct.mode = GPIO_MODE_AF;
        gpio_init_struct.speed = GPIO_SPEED_100MHz;
        gpio_init_struct.otype = GPIO_OTYPE_PP;
        gpio_init_struct.pupd = GPIO_PUPD_UP;
        GPIO_Config(GPIOD,&gpio_init_struct);
    
        gpio_init_struct.pin = GPIO_PIN_10;
        gpio_init_struct.mode = GPIO_MODE_OUT;
        gpio_init_struct.speed = GPIO_SPEED_100MHz;
        gpio_init_struct.otype = GPIO_OTYPE_PP;
        gpio_init_struct.pupd = GPIO_PUPD_UP;
        GPIO_Config(GPIOC,&gpio_init_struct);
    
        NVIC_EnableIRQRequest(UART5_IRQn, 3, 1);       
    }

    if(USARTx == USART6)
    {
        /* 使能时钟 */
        RCM_EnableAHB1PeriphClock(RCM_AHB1_PERIPH_GPIOC);
        RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_USART6);

        GPIO_ConfigPinAF(GPIOC, GPIO_PIN_SOURCE_6, GPIO_AF_USART6);
        GPIO_ConfigPinAF(GPIOC, GPIO_PIN_SOURCE_7, GPIO_AF_USART6);

        gpio_init_struct.pin = GPIO_PIN_6;
        gpio_init_struct.mode = GPIO_MODE_AF;
        gpio_init_struct.speed = GPIO_SPEED_100MHz;
        gpio_init_struct.otype = GPIO_OTYPE_PP;
        gpio_init_struct.pupd = GPIO_PUPD_NOPULL;
        GPIO_Config(GPIOC, &gpio_init_struct);    

        gpio_init_struct.pin = GPIO_PIN_7;
        gpio_init_struct.mode = GPIO_MODE_AF;
        gpio_init_struct.speed = GPIO_SPEED_100MHz;
        gpio_init_struct.otype = GPIO_OTYPE_PP;
        gpio_init_struct.pupd = GPIO_PUPD_UP;
        GPIO_Config(GPIOC,&gpio_init_struct);
    
        gpio_init_struct.pin = GPIO_PIN_8;
        gpio_init_struct.mode = GPIO_MODE_OUT;
        gpio_init_struct.speed = GPIO_SPEED_100MHz;
        gpio_init_struct.otype = GPIO_OTYPE_PP;
        gpio_init_struct.pupd = GPIO_PUPD_UP;
        GPIO_Config(GPIOC,&gpio_init_struct);
    
        NVIC_EnableIRQRequest(USART6_IRQn, 3, 0);
    }

    usart_init_struct.baudRate = baudrate;                      /* 通讯波特率 */
    usart_init_struct.wordLength = USART_WORD_LEN_8B;           /* 数据位     */
    usart_init_struct.stopBits = USART_STOP_BIT_1;              /* 停止位     */
    usart_init_struct.parity = USART_PARITY_NONE;               /* 校验位     */
    usart_init_struct.mode = USART_MODE_TX_RX;                  /* 收发模式   */
    usart_init_struct.hardwareFlow = USART_HARDWARE_FLOW_NONE;  /* 无硬件流控 */

    USART_Config(USARTx, &usart_init_struct);
    USART_Enable(USARTx);

    USART_EnableInterrupt(USARTx, USART_INT_RXBNE);
    USART_ClearStatusFlag(USARTx, USART_FLAG_RXBNE);

    RS485_TX_EN(1);		    // 默认为接收模式
    RS485_TX1_EN(1);        // 默认为接收模式
}

/**
 * @brief RS485发送len个字节.
 * 
 * @param buf 发送区首地址
 * @param len 发送的字节数(为了和本代码的接收匹配,这里建议不要超过64个字节)
 */
void RS485_Send_Data(u8 *buf,u8 len)
{
	u8 t;
	RS485_TX_EN(0);			           // 设置为发送模式

    for(t=0;t<len;t++)		           // 循环发送数据
    {
        USART_TxData(USART6,*(buf+t)); // 发送数据
        while(!USART_ReadStatusFlag(USART6, USART_FLAG_TXBE));      // 判断一个数据帧是否发送完成
    }
 
	while(USART_ReadStatusFlag(USART6, USART_FLAG_TXBE) == RESET);

	RS485_RX_CNT=0;	  
	RS485_TX_EN(1);				       // 设置为接收模式	
}

/**
 * @brief RS485查询接收到的数据
 * 
 * @param buf 接收缓存首地址
 * @param len 读到的数据长度
 */
void RS485_Receive_Data(u8 *buf,u8 *len)
{
	u8 rxlen=RS485_RX_CNT;
	u8 i=0;
	*len=0;				    // 默认为0
	delay_ms(10);		    // 等待10ms,连续超过10ms没有接收到一个数据,则认为接收结束
	if(rxlen==RS485_RX_CNT&&rxlen)// 接收到了数据,且接收完成了
	{
		for(i=0;i<rxlen;i++)
		{
			buf[i]=RS485_RX_BUF[i];	
		}		
		*len=RS485_RX_CNT;	// 记录本次数据长度
		RS485_RX_CNT=0;		// 清零
	}
}

/**
 * @brief 打包需要发送的数据
 * 
 * @param ibuf 需要传入的数组
 * @param DmxData 需要发送的数组
 * @param DmxLen 数组长度
 */
void PackUploadDmxData(uint8_t* ibuf,uint8_t* DmxData,uint8_t DmxLen)
{
    ibuf[0] = 0x1;                      // 包头为20个字节 第一个字节为0x1，剩下19个字节为0xFF
    memset(&ibuf[1],0xFF,19);             

    memcpy(&ibuf[20],DmxData,DmxLen);   // 复制需要发送的数据到ibuf中（从ibuf[20]开始）
}

/**********************************************应用层***********************************************************/

/**
 * @brief 初始化RS485 MQ320有两个485（串口6和串口5）
 * 
 */
void InitRs485s(void)
{ 
    InitRs485(USART6,9600);  // 初始化串口6
    InitRs485(UART5,9600);   // 初始化串口5
}

/**
 * @brief Rs485接收并解析指令，指令符合要求后发送完成标志位
 * 
 * @param ibuf 传入需要解析的数组
 * @param tLen 数组的长度
 */
void Rs485FinishCmd(uint8_t *ibuf,uint8_t tLen)
{
    uint8_t arrLen = tLen;
    uint8_t arr[40] = {0};
    uint8_t cnt = 0;

    memcpy(arr,ibuf,arrLen);

    if(arr[0] == 0x01)
    {
        cnt = 1;
        while(cnt != 19)
        {
            if(arr[cnt] == 0xFF)   
            {
                cnt++;
            }     
            else
            {
                break;
            }
        }
        if(arr[20] == 0xA5 && arr[21] == 0x0A && arr[22] == 0x0B && arr[23] == 0x0C && arr[24] == 0x5A)
        {
            SetDmxRevFinish(TRUE);   
        }       
        else
        {
            RS485_RX_CNT = 0;
        } 
    }
}

/**
 * @brief Dmx发送处理任务（放在主函数当中）
 * 
 */
void DmxHandleTask(void)
{
    uint8_t DmxData[64] = {1,2,3,4,5,6};
    uint8_t rs485Rev[64] = {0};
    uint8_t revLen = 0;
  
    RS485_Receive_Data(rs485Rev,&revLen);

    if(revLen == REV_DMX_LEN)
    {
        Rs485FinishCmd(rs485Rev,revLen); 
    }

    if(GetDmxRevFinish())
    {
        SetDmxRevFinish(FALSE);         
        PackUploadDmxData(DmxDataPack,DmxData,6);
        RS485_Send_Data(DmxDataPack,26);
        memset(&RS485_RX_BUF,0,64);
        memset(&DmxDataPack,0,128);
    }
}

/**
 * @brief Set the Dmx Rev Finish object
 * 
 * @param Set Finish Flag
 */
void SetDmxRevFinish(u8 val)
{
	uart2RevFinish = val;
}

/**
 * @brief Get the Dmx Rev Finish object
 * 
 * @return Finish Flag
 */
u8 GetDmxRevFinish(void)
{
	return uart2RevFinish;
}


/*************串口中断函数***************/

/**
 * @brief 串口6中断函数，接收Rs485收到的数据
 * 
 */
void USART6_IRQHandler(void)
{
	uint8_t res;	
        
    if(USART_ReadStatusFlag(USART6,USART_FLAG_OVRE) != RESET)
    {
        USART_ClearStatusFlag(USART6,USART_FLAG_OVRE);
        USART_RxData(USART6);
    }
    if(USART_ReadStatusFlag(USART6,USART_FLAG_RXBNE) == SET)
    {
        USART_ClearIntFlag(USART6,USART_INT_RXBNE);
	 			 
		res = USART_RxData(USART6);	            // 读取接收到的数据
		if(RS485_RX_CNT<64)
		{
			RS485_RX_BUF[RS485_RX_CNT]=res;		// 记录接收到的值
			RS485_RX_CNT++;						// 接收数据增加1 
		} 
	}  											 
} 
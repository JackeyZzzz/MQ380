/**
 * @file usart.c
 * @author JackeyZ
 * @brief (1) ����:		���ڴ�����ģ����ص�����
 * @version 0.1
 * @date 2023-12-28
 * 
 * @copyright Copyright (c) 2023 SEEKWAY
 * 
 */
#include "includes.h"

/* ����2�жϷ����ӳ������ڽ���GPS���� */	
volatile uint8_t Time2SendGPSData = RESET;					// ����GPS���ݵ�PC
volatile uint8_t gps_index = 0;								// GPS����������������
volatile uint8_t GPS_Buf[GPS_BUF_SIZE] = {0};				// GPS������
uint8_t g_serialRxPacket[100];				                // ����������ݰ����飬���ݰ���ʽ"@MSG\r\n"


/* ���ڽ��ջ����� */
uint8_t g_usart_rx_buf[USART_REC_LEN];

/* ���ڽ���״̬
 * bit15  : ������ɱ�־
 * bit14  : ���յ�0x0D
 * bit13~0: ���յ�����Ч�ֽ���
 */
uint16_t g_usart_rx_sta = 0;

/* ���ڽ�����ʱ���� */
uint8_t g_rx_buffer[RXBUFFERSIZE];

/**
 * @brief �����õ��Ĵ��ں���
 * 
 * @note ����һ������Ϊ115200 ���ڶ�������Ϊ9600
 */
void InitUsarts(void)
{ 
    USARTx_Init(USART1,115200); // ��ʼ������1
    USARTx_Init(UART4,9600);    // ��ʼ������2
}

/**
 * @brief ���ڳ�ʼ��
 * 
 * @param USARTx ���ô���ͨ��
 * @param baudrate ���ò�����
 */
void USARTx_Init(USART_T *USARTx,uint32_t baudrate)
{
    GPIO_Config_T gpio_init_struct;
    USART_Config_T usart_init_struct;
    
    if(USARTx == USART1)
    {
        /* ʹ��ʱ�� */
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
        /* ʹ��ʱ�� */
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
   
    usart_init_struct.baudRate = baudrate;                      /* ͨѶ������ */
    usart_init_struct.wordLength = USART_WORD_LEN_8B;           /* ����λ */
    usart_init_struct.stopBits = USART_STOP_BIT_1;              /* ֹͣλ */
    usart_init_struct.parity = USART_PARITY_NONE;               /* У��λ */
    usart_init_struct.mode = USART_MODE_TX_RX;                  /* �շ�ģʽ */
    usart_init_struct.hardwareFlow = USART_HARDWARE_FLOW_NONE;  /* ��Ӳ������ */

    USART_Config(USARTx, &usart_init_struct);
    USART_Enable(USARTx);

    USART_EnableInterrupt(USARTx, USART_INT_RXBNE);
    USART_ClearStatusFlag(USARTx, USART_FLAG_RXBNE);
}

/**
 * @brief USARTx send one byte
 * @param USART_TypeDef* ���÷��͵�ͨ��
 * @param u8 ch ��Ҫ���͵�����
 * 
 */
void USARTx_Send_Char(USART_T* USARTx,u8 ch)
{
	USART_TxData(USARTx,ch);//��������
	while(!USART_ReadStatusFlag(USARTx, USART_FLAG_TXBE));//�ж�һ������֡�Ƿ������
}

/**
 * @brief ����x�����ַ���
 * @param USART_TypeDef* USARTx
 * @param u8 ch
 * 
 */
void USARTx_Send_String(USART_T* USARTx,char *str)
{
	while(*str!='\0')//�ж��ַ����Ƿ������
	{
		USART_TxData(USARTx,*str);//��������
		while(!USART_ReadStatusFlag(USARTx, USART_FLAG_TXBE));//�ж�һ������֡�Ƿ������
		str++;//ָ��ƫ��
	}
}

/*
�������ܣ�����x����ָ����������
������USART_TypeDef* USARTx,u8 *data_buff,u16 data_len
����ֵ����
*/
void USARTx_Send_Buff(USART_T* USARTx,u8 *data_buff,u16 data_len)
{
    for(u16 i=0;i<data_len;i++)
    {
        USART_TxData(USARTx,*(data_buff+i));//��������
        while(!USART_ReadStatusFlag(USARTx, USART_FLAG_TXBE));//�ж�һ������֡�Ƿ������
    }
}



/*************�����жϺ���***************/

/**
 * @brief ����1�жϷ����� ��Ҫ���ڵ���
 * 
 */
void USART1_IRQHandler(void)                                 // ����һ������
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
 * @brief       ����4�жϷ�����
 * @note        ���ڽ���GPSģ�鷢�͵����� 
 *              GPS_Buf��GPS�������ݻ����� 
 *              gps_index��GPS�������ݳ���
 * @param       ��
 * @retval      ��
 */

void UART4_IRQHandler(void)                                  // GPS����ʹ��
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

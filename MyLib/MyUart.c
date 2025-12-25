#include "MyUart.h"

// USART1中断接收相关变量
static uint8_t Data;          // 当前接收的数据字节
static uint8_t Rx_FLAG = 0;   // 接收完成标志
static uint8_t RX_Str[128];   // 接收数据缓冲区
/*********************************************************************
 * @fn      USART1_Config
 * @brief   初始化USART1
 * @param   BaudRate - 波特率，可以是115200,9600
 * @return  none
 ********************************************************************/
void USART1_Config(uint32_t BaudRate)
{
    // TX - PA9, RX - PA10
    GPIO_InitTypeDef  GPIO_InitStructure = {0};

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  // 串口Tx必须配置为复用推挽
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  // 串口Rx配置为浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // USART1 初始化配置
    USART_InitTypeDef USART_InitStructure = {0};
    USART_InitStructure.USART_BaudRate = BaudRate;        // 115200
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;  // 同时使能发送和接收
    USART_Init(USART1, &USART_InitStructure);

    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);  // 使能接收数据寄存器非空中断, 当接收到数据时触发中断
    // NVIC
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    NVIC_InitTypeDef NVIC_InitStructure = {0};
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;   // USART1 中断通道
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;   // 抢占优先级0
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  // 响应优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    USART_Cmd(USART1, ENABLE);
}

/**（名称，参数，返回值）
  * @brief  发送一个字节
  * @param USARTx
  * @param 一个字节 
  * @retval None
  */
//USART_TypeDef* pUSARTx---p表示区分指针
void Usart_SendByte(USART_TypeDef* pUSARTx,uint8_t date)
{
	USART_SendData(pUSARTx, date);
	//USART_FLAG_TXE一直为0，表示字节发送出去了
	while(USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
}

/**
  * @brief  发送两个字节
  * @param USARTx
  * @param 两个字节 
  * @retval None
  */
void Usart_SendHalfWord(USART_TypeDef* pUSARTx,uint16_t date)
{
	int8_t temp_h,temp_l;	//高8低8
	//取数
	temp_h = (date & 0xff00)>>8;
	temp_l = date & 0xff;
	
	USART_SendData(pUSARTx, temp_h);
	//USART_FLAG_TXE一直为0，表示字节发送出去了
	while(USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
	
	USART_SendData(pUSARTx, temp_l);
	while(USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
}
/**
  * @brief  发送数组
  * @param USARTx
  * @param 两个字节 
  * @retval None
  */
void Usart_SendString( USART_TypeDef * pUSARTx, char *str)
{
	unsigned int k=0;
	do{
		Usart_SendByte( pUSARTx, *(str + k) );
		k++;
	} while (*(str + k)!='\0');
	//等待发送完成
	while (USART_GetFlagStatus(pUSARTx,USART_FLAG_TC)==RESET) 
	{
	}
}

//重定向 c 库函数 printf 到串口，重定向后可使用 printf 函数
int fputc(int ch, FILE *f)
{
	USART_SendData(USART1, (uint8_t) ch);								//发送一个字节数据到串口
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);		//等待发送完毕
	return (ch);
 }
 
//重定向 c 库函数 scanf 到串口，重写向后可使用 scanf、getchar 等函数
int fgetc(FILE *f)
{
	while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);			//等待串口输入数据
	return (int)USART_ReceiveData(USART1);
}

void RX_Process(void)
{
    if(Rx_FLAG == 1)
    {
        // 判断接收到的指令
        if(strcmp((char*)RX_Str, "LED_ON") == 0)
        {
            LED_On();
            printf("LED_ON\n");
        }
        else if(strcmp((char*)RX_Str, "LED_OFF") == 0)
        {
            LED_Off();
            printf("LED_OFF\n");
        }
        else
        {
            printf("Unknown command\n");
        }
        
        // 清除接收标志位，准备下一次接收
        Rx_FLAG = 0;
    }
}

//-----------------------------------------------------------------------------------------
/**
  * @brief  串口1中断服务函数
  * @retval None
  */
void USART1_IRQHandler(void)    //串口中断函数
{
	static u16 RX_Status;		//串口接收状态
	static u16 i;			//字符数组下标
	if (USART_GetITStatus(USART1, USART_IT_RXNE)==SET)//当接收标志位为SET 开始接收
	{
		Data=USART_ReceiveData(USART1);//接收数据
		if(RX_Status==0)					//若串口接收状态为状态0
		{
			if(Data=='@'&&Rx_FLAG==0)//判断接收到的数据是否为包头 @ 在Rx_FLAG=0情况下接收数据防止数据传输过快错乱
			{
				RX_Status=1;			//转换为状态1
				i=0;				//字符数组下标置0
			}
		}
		// @ DATA \r\n
        else if(RX_Status==1)			//若串口接收状态为状态1
		{
			if(Data=='\r')			//判断是否为包尾字符 不定长HEX数据包接收数据需时刻比对包尾信号
			{
				RX_Status=2;			//转换为状态2
			}
			else
			{
				RX_Str[i]=Data;		//接收数据存入字符数组
				i++;
			}
		}
        else if(RX_Status==2)			//若串口接收状态为状态2
		{
			if(Data=='\n')			//再次判断是否为包尾字符
			{
				RX_Status=0;			//转换为状态0
				RX_Str[i]='\0';		//字符串接收结束
				Rx_FLAG=1;			//接收完成标志位置1
			}
		}
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);//清除接收标志位
	}
}
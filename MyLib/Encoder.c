#include "Encoder.h"

// 消抖相关定义
#define SAMPLE_COUNT 3  // 采样次数

// 全局变量
int16_t Encoder_Count;
volatile uint8_t flag_a = 0;  // A相（PB0）中断标志位
volatile uint8_t flag_b = 0;  // B相（PB1）中断标志位
// 状态变量（主循环使用，无需volatile）
uint8_t last_ab = 0;          // 上一次的AB相状态（2位：bit1=A相，bit0=B相）
int16_t Encoder_Total = 0;    // 累计计数（左转累加、右转减小）
// 消抖变量
uint8_t sample_buffer[SAMPLE_COUNT];
uint8_t sample_index = 0;
void Encoder_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruture;
	GPIO_InitStruture.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruture.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStruture.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruture);
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource0);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource1);
	
	EXTI_InitTypeDef EXTI_InitStructure;
	EXTI_InitStructure.EXTI_Line = EXTI_Line0 | EXTI_Line1;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_Init(&EXTI_InitStructure);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&NVIC_InitStructure);
}
 
// 多数表决函数
uint8_t majority_vote(uint8_t* buffer, uint8_t count)
{
    uint8_t vote_00 = 0, vote_01 = 0, vote_10 = 0, vote_11 = 0;
    
    for (uint8_t i = 0; i < count; i++)
    {
        switch (buffer[i])
        {
            case 0b00: vote_00++; break;
            case 0b01: vote_01++; break;
            case 0b10: vote_10++; break;
            case 0b11: vote_11++; break;
        }
    }
    
    // 返回得票最多的状态
    if (vote_00 >= vote_01 && vote_00 >= vote_10 && vote_00 >= vote_11) return 0b00;
    if (vote_01 >= vote_00 && vote_01 >= vote_10 && vote_01 >= vote_11) return 0b01;
    if (vote_10 >= vote_00 && vote_10 >= vote_01 && vote_10 >= vote_11) return 0b10;
    return 0b11;
}

void Encoder_Process(void)
{
    uint8_t current_ab;
    uint8_t stable_state;
	
    // 检查A相或B相中断标志位（有事件发生）
    if (flag_a || flag_b)
    {
        // 读取当前AB相状态（2位：bit1=A相，bit0=B相）
        current_ab = (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0) << 1) | GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1);
        
        // 将当前状态加入采样缓冲区
        sample_buffer[sample_index] = current_ab;
        sample_index = (sample_index + 1) % SAMPLE_COUNT;
        
        // 当缓冲区填满时进行多数表决
        if (sample_index == 0)
        {
            // 获取稳定状态
            stable_state = majority_vote(sample_buffer, SAMPLE_COUNT);
            
            // 只有当稳定状态与上一次状态不同时，才认为是有效旋转
            if (stable_state != last_ab)
            {
                // 判断旋转方向（基于增量式编码器相位差规则）
                switch ((last_ab << 2) | stable_state)  // 组合上一次+当前状态（4位）
                {
                    // 右转（顺时针）：有效状态跳转 → 减小（-1）
                    case 0b0001: case 0b0111: case 0b1110: case 0b1000:
                        Encoder_Total--;
                        break;
                    // 左转（逆时针）：有效状态跳转 → 累加（+1）
                    case 0b0010: case 0b1011: case 0b1101: case 0b0100:
                        Encoder_Total++;
                        break;
                    // 无效状态跳转（如抖动）→ 不处理
                    default:
                        break;
                }
                last_ab = stable_state;  // 更新上一次状态
            }
        }
		
        // 清除中断标志位
        flag_a = 0;
        flag_b = 0;
    }
}

// 获取累计计数（供主函数调用）
int16_t Encoder_Get(void)
{
    Encoder_Process();
    return Encoder_Total;
}
	
void EXTI0_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line0)  == SET)
	{
         flag_a = 1;  // 设置A相中断标志位
		// if (GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1) == 0)
		// {
		// 	Encoder_Count--;
		// }
		EXTI_ClearITPendingBit(EXTI_Line0);
	}
}
 
void EXTI1_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line1)  == SET)
	{
        flag_b = 1;  // 设置B相中断标志位               
		// if (GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_0) == 0)
		// {
		// 	Encoder_Count++;
		// }
		EXTI_ClearITPendingBit(EXTI_Line1);
	}   
}
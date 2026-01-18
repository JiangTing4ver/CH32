#include "Encoder.h"

// 消抖相关定义
#define STATE_IDLE       0
#define STATE_DETECTING  1
#define STATE_VALID      2
#define DEBOUNCE_DELAY   5  // 消抖延迟（ms）

// 全局变量
int16_t Encoder_Count;
volatile uint8_t flag_a = 0;  // A相（PB0）中断标志位
volatile uint8_t flag_b = 0;  // B相（PB1）中断标志位
// 状态变量（主循环使用，无需volatile）
uint8_t last_ab = 0;          // 上一次的AB相状态（2位：bit1=A相，bit0=B相）
int16_t Encoder_Total = 0;    // 累计计数（左转累加、右转减小）
// 消抖状态机变量
uint8_t debounce_state = STATE_IDLE;
uint8_t detected_state = 0;
uint32_t detect_start_time = 0;
// 系统时间变量
volatile uint32_t system_tick = 0;

// 系统tick中断处理函数
void SysTick_Handler(void)
{
    system_tick++;
}

// 获取系统时间（毫秒）
uint32_t GetSystemTick(void)
{
    return system_tick;
}

// 初始化系统tick
void SystemTick_Init(void)
{
    // 配置SysTick为1ms中断
    SysTick_Config(SystemCoreClock / 1000);
}
void Encoder_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	
	// 初始化系统tick
	SystemTick_Init();
	
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
 
void Encoder_Process(void)
{
    uint8_t current_ab;
    uint32_t current_time;
	
    // 检查A相或B相中断标志位（有事件发生）
    if (flag_a || flag_b)
    {
        // 读取当前时间
        current_time = GetSystemTick();
        
        // 读取当前AB相状态（2位：bit1=A相，bit0=B相）
        current_ab = (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0) << 1) | GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1);
        
        // 状态机处理
        switch (debounce_state)
        {
            case STATE_IDLE:
                // 检测到状态变化，开始消抖
                if (current_ab != last_ab)
                {
                    detected_state = current_ab;
                    detect_start_time = current_time;
                    debounce_state = STATE_DETECTING;
                }
                break;
                
            case STATE_DETECTING:
                // 检查消抖时间是否达到
                if (current_time - detect_start_time >= DEBOUNCE_DELAY)
                {
                    // 状态稳定，判断是否与检测状态一致
                    if (current_ab == detected_state)
                    {
                        // 有效状态，判断旋转方向
                        switch ((last_ab << 2) | current_ab)  // 组合上一次+当前状态（4位）
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
                        last_ab = current_ab;  // 更新上一次状态
                        debounce_state = STATE_VALID;
                    }
                    else
                    {
                        // 状态不稳定，重新检测
                        detected_state = current_ab;
                        detect_start_time = current_time;
                    }
                }
                break;
                
            case STATE_VALID:
                // 等待下一次状态变化
                if (current_ab != last_ab)
                {
                    detected_state = current_ab;
                    detect_start_time = current_time;
                    debounce_state = STATE_DETECTING;
                }
                break;
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
#include "ch32f10x.h"

// 延时函数
void Delay(uint32_t nCount)
{
    for(; nCount != 0; nCount--);
}

int main(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // 初始化系统时钟
    SystemInit();
    
    // 使能GPIOC时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    
    // 配置PC13引脚为推挽输出
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    
    // 点亮LED（PC13低电平有效）
    GPIO_ResetBits(GPIOC, GPIO_Pin_13);
    
    while(1)
    {
        // 程序主循环
        Delay(0xFFFFF);
        // 这里可以添加LED闪烁代码，如果需要的话
        // GPIO_SetBits(GPIOC, GPIO_Pin_13);
        // Delay(0xFFFFF);
        // GPIO_ResetBits(GPIOC, GPIO_Pin_13);
    }
}
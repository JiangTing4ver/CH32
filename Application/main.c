#include "ch32f10x.h"
#include "delay.h"


int main(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // 初始化系统时钟
    SystemInit();
    
    // 初始化延时功能
    Delay_Init();
    
    // 使能GPIOC时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    
    // 配置PC13引脚为推挽输出
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    
    
    while(1)
    {
        GPIO_SetBits(GPIOC, GPIO_Pin_13);
        Delay_Ms(100);  // 延时100毫秒（0.5秒）
        GPIO_ResetBits(GPIOC, GPIO_Pin_13);
        Delay_Ms(100);  // 延时100毫秒（0.5秒）
    }
}
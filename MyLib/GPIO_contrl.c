#include "GPIO_contrl.h"

void GPIO_OUT_SetInit(uint32_t RCC_Periph_GPIOx, GPIO_TypeDef* GPIO_Port, uint16_t GPIO_Pin)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // 使能GPIOC时钟
    RCC_APB2PeriphClockCmd(RCC_Periph_GPIOx, ENABLE);
    
    // 配置PC13引脚为推挽输出
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    // 推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIO_Port, &GPIO_InitStructure);  
}

void GPIO_IN_SetInit(uint32_t RCC_Periph_GPIOx, GPIO_TypeDef* GPIO_Port, uint16_t GPIO_Pin)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // 使能GPIOC时钟
    RCC_APB2PeriphClockCmd(RCC_Periph_GPIOx, ENABLE);
    
    // 配置PC13引脚为浮空输入
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;       // 浮空输入
    GPIO_Init(GPIO_Port, &GPIO_InitStructure);  
}


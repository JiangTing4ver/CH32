#include "ch32f10x.h"
#include "GPIO_contrl.h"
#include "delay.h"


int main(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // 初始化系统时钟
    SystemInit();
    
    // 初始化延时功能
    Delay_Init();
    
    // 初始化GPIOC引脚
    GPIO_SetInit(RCC_Periph_GPIO, GPIO_Port_GPIO, GPIO_Pin_GPIO);
    
    
    while(1)
    {
        GPIO_SetBits(GPIOC, GPIO_Pin_13);
        Delay_Ms(10); 
        GPIO_ResetBits(GPIOC, GPIO_Pin_13);
        Delay_Ms(10);
    }
}
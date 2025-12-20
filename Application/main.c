#include "ch32f10x.h"
#include "GPIO_contrl.h"
#include "LED.h"



int main(void)
{
    
    // 初始化系统时钟
    SystemInit();
    
    // 初始化延时功能
    Delay_Init();
    
    // 初始化LED
    LED_Init();
    
    while(1)
    {
        LED_blink();
    }
}
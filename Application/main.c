#include "HOME.h"

#define LED 0
#define HM 1

// #define flag LED  // 使用LED功能
#define flag HM     // 使用传感器功能


int main(void)
{
    
    // 初始化系统时钟
    SystemInit();
    
    // 初始化延时功能
    Delay_Init();

    #if flag == LED
        // 初始化LED
        LED_Init();
    #elif flag == HM
        // 初始化传感器
        MH_Sersor_Init();
    #endif
    
    while(1)
    {
        #if flag == LED
            LED_blink();
        #elif flag == HM
            Note();
        #endif  
    }
}
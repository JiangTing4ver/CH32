#include "../MyLib/HOME.h"

#define LED 0
#define HM 1
#define GY520 2
#define MyUSART1 3
#define WS2812 4

// #define flag LED  // 使用LED功能
// #define flag HM     // 使用传感器功能
// #define flag GY520  // 使用GY520功能
// #define flag MyUSART1 // 使用USART1功能
#define flag WS2812  // 使用WS2812功能

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
    #elif flag == GY520
        // 初始化GY520
        GY520_Init();
    #elif flag == MyUSART1
        // 初始化USART1
        USART1_Config(115200);
        // 初始化LED
        LED_Init();
        printf("this a example for USART1 :) \n");
    #elif flag == WS2812
        // 初始化WS2812
        WS2812_Init();
        // PA8_test();
    #endif
    
    while(1)
    {
        #if flag == LED
            LED_blink();
        #elif flag == HM
            Note();
        #elif flag == GY520
            GY520_Update();
        #elif flag == MyUSART1
            RX_Process();
        #elif flag == WS2812
            WS2812_RainbowWaterFlowEffect(200);
        #endif  
    }
}
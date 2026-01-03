#include "../MyLib/HOME.h"

#define LED 0
#define HM 1
#define MyUSART1 2
#define WS2812 3
#define I2C 4

// #define flag LED  // 使用LED功能
// #define flag HM     // 使用传感器功能
// #define flag MyUSART1 // 使用USART1功能
// #define flag WS2812  // 使用WS2812功能
#define flag I2C

int main(void)
{
    
    SystemInit();
    SystemCoreClockUpdate();
    Delay_Init();

    #if flag == LED
        // 初始化LED
        LED_Init();
    #elif flag == HM
        // 初始化传感器
        MH_Sersor_Init();
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
    #elif flag == I2C
        LED_Init();
        // IIC_Init(I2C_SPEED, TxAddress);
        I2C_Soft_Init();
    #endif
    
    while(1)
    {
        #if flag == LED
            LED_blink(500);
        #elif flag == HM
            Note();
        #elif flag == MyUSART1
            RX_Process();
        #elif flag == WS2812
            // WS2812_RainbowWaterFlowEffect(200);
            WS2812_BreathingLight(8, 255, 0, 0, 50);
            WS2812_BreathingLight(8, 0, 0, 255, 200);
            WS2812_BreathingLight(8, 0, 255, 0, 100);
            WS2812_BreathingLight(8, 0, 255, 255, 80);
        #elif flag == I2C
            // // 发送数据5次，每次间隔1秒
            // for(uint8_t send_count = 0; send_count < 5; send_count++)
            // {
            //     printf("Send %d times: 01 02 03 04 05 06\r\n", send_count + 1);
            //     uint8_t result = I2C_Host_SendData(0x02, TxData, Size);  // 使用7位地址
            //     if(result == 0)
            //     {
            //         printf("Send successful\r\n");
            //     }
            //     else
            //     {
            //         printf("Send failed\r\n");
            //     }
            //     Delay_Ms(1000);  // 延时1秒
            // }

            for(uint8_t send_count = 0; send_count < 5; send_count++)
            {
                printf("Send %d times: 01 02 03 04 05 06\r\n", send_count + 1);
                I2C_Soft_SendData(SlaveAddr, TxData, Size);  // 软件I2C发送
                Delay_Ms(10);  // 间隔1秒
            }
            while(1)
            {
                LED_blink(500);  // 数据发送完成后，LED持续闪烁
            }
        #endif  
    }
}
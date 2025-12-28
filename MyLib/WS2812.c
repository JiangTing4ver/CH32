#include "WS2812.h"

// PA8
#define GPIO_Port_GPIO GPIOA
#define GPIO_Pin_GPIO GPIO_Pin_8

// Number of WS2812 LEDs in the strip
#define WS2812_NUM_LEDS 8 

// WS2812 timing values (based on 8MHz clock, 90 counts = 1.25us period)
#define WS2812_ONE_HIGH 60   // 0.8us high for 1 bit
#define WS2812_ZERO_HIGH 30  // 0.4us high for 0 bit

// LED buffer
static WS2812_Color WS2812_ColorBuf[WS2812_NUM_LEDS];

void TIM1_CH1_Init(uint16_t psc, uint16_t arr, uint16_t ccr)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;

    // 使能TIM1时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1 , ENABLE);

    TIM_TimeBaseStructure.TIM_Prescaler = psc - 1;           // 预分频器
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // 向上计数模式
    TIM_TimeBaseStructure.TIM_Period = arr - 1;             // 自动重载值
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; // 时钟分频：不分频
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;   // 重复计数器：0（高级定时器特有，关闭）
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

    // 配置PWM模式（CH1）
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;  // PWM模式1：CNT < CCR时输出有效电平
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; // 使能CH1输出
    TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable; // 关闭互补输出（无CH1N）
    TIM_OCInitStructure.TIM_Pulse = ccr;                
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; // 输出极性：高电平有效
    TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High; // 互补输出极性（未使用）
    TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset; // 空闲状态：复位
    TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset; // 互补空闲状态（未使用）
    TIM_OC1Init(TIM1, &TIM_OCInitStructure);            // 初始化CH1

    TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);   // 使能CH1比较值预装载
    TIM_ARRPreloadConfig(TIM1, ENABLE);                 // 使能自动重载预装载

    TIM_CtrlPWMOutputs(TIM1, ENABLE);

    TIM_Cmd(TIM1, ENABLE);
}

void WS2812_Pin_init()
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP; // 复用推挽输出
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_GPIO;
    GPIO_Init(GPIO_Port_GPIO, &GPIO_InitStruct);
    // 如果不需要重映射，可以删除下面这行；若需部分重映射，请填入 GPIO_PartialRemap_TIM1
    // GPIO_PinRemapConfig(GPIO_PartialRemap_TIM1, ENABLE);
}


void WS2812_Init(void)
{
    TIM1_CH1_Init(1, 90, 0); 
    WS2812_Pin_init();
}

void send_1(void)
{
    TIM_SetCompare1(TIM1, 60);
}

void send_0(void)
{
    TIM_SetCompare1(TIM1, 30);
}

void WS2812_SendByte(uint8_t byte)
{
    uint8_t i;
    for (i = 0; i < 8; i++)
    {
        if (byte & 0x80)  // 检查当前最高位
        {
            send_1();
        }
        else
        {
            send_0();
        }
        byte <<= 1;  // 左移一位，处理下一个位
    }
}

// Send a single byte to WS2812
void WS2812_SendByte(uint8_t byte)
{
    uint8_t i;
    // Send bits from MSB to LSB
    for (i = 0; i < 8; i++)
    {
        if (byte & 0x80)  // Check MSB
        {
            send_1();
        }
        else
        {
            send_0();
        }
        byte <<= 1;  // Shift to next bit
    }
}

// Update all LEDs (send latch signal >50us low)
void WS2812_Show(void)
{
    uint16_t i;
    
    // Send color data for all LEDs in the buffer
    for(i = 0; i < WS2812_NUM_LEDS; i++) 
    {
        WS2812_SendByte(WS2812_ColorBuf[i].g);
        WS2812_SendByte(WS2812_ColorBuf[i].r);
        WS2812_SendByte(WS2812_ColorBuf[i].b);
    }
    
    // Set PWM to 0% duty cycle for the latch signal
    TIM_SetCompare1(TIM1, 0);
    
    Delay_Us(100);
}

void WS2812_SetLED(uint16_t ledIndex, uint8_t red, uint8_t green, uint8_t blue)
{
    // Check if the LED index is valid
    if (ledIndex < WS2812_NUM_LEDS) 
    {
        // Store the color in the buffer (GRB format)
        WS2812_ColorBuf[ledIndex].g = green;
        WS2812_ColorBuf[ledIndex].r = red;
        WS2812_ColorBuf[ledIndex].b = blue;
    }
}

WS2812_Color WS2812_CreateColor(uint8_t red, uint8_t green, uint8_t blue)
{
    WS2812_Color color;
    color.r = red;
    color.g = green;
    color.b = blue;
    return color;
}


void WS2812_SendColor(WS2812_Color color)
{
    WS2812_SendByte(color.g);
    WS2812_SendByte(color.r);
    WS2812_SendByte(color.b);
}

// Clear all LEDs
void WS2812_Clear(void)
{
    // Set all LEDs in the buffer to black (off)
    uint16_t i;
    for(i = 0; i < WS2812_NUM_LEDS; i++) 
    {
        WS2812_ColorBuf[i].g = 0;
        WS2812_ColorBuf[i].r = 0;
        WS2812_ColorBuf[i].b = 0;
    }
    
    WS2812_Show();
}

// Fill all LEDs with the same color
void WS2812_Fill(WS2812_Color color)
{
    // Fill the buffer with the same color for all LEDs
    uint16_t i;
    for(i = 0; i < WS2812_NUM_LEDS; i++) 
    {
        WS2812_ColorBuf[i] = color;
    }
    
    // Update all LEDs with the new color data
    WS2812_Show();
}

void WS2812_SetLEDColor(uint16_t ledIndex, WS2812_Color color)
{
    // Check if the LED index is valid
    if (ledIndex < WS2812_NUM_LEDS) 
    {
        // Store the color in the buffer (GRB format)
        WS2812_ColorBuf[ledIndex] = color;
    }
}

// Water flow effect with specified color
void WS2812_WaterFlowEffect(WS2812_Color color, uint16_t speedMs)
{
    uint16_t i, j;
    
    // Clear all LEDs initially
    WS2812_Fill(WS2812_CreateColor(0, 0, 0));
    WS2812_Show();
    Delay_Ms(500);
    
    // Create flowing water effect
    for (i = 0; i < WS2812_NUM_LEDS + 3; i++)
    {
        // Clear buffer
        for (j = 0; j < WS2812_NUM_LEDS; j++)
        {
            WS2812_ColorBuf[j].r = 0;
            WS2812_ColorBuf[j].g = 0;
            WS2812_ColorBuf[j].b = 0;
        }
        
        // Set current LED and trailing LEDs with decreasing brightness
        if (i < WS2812_NUM_LEDS)
        {
            WS2812_SetLEDColor(i, color);
        }
        
        if (i - 1 >= 0 && i - 1 < WS2812_NUM_LEDS)
        {
            WS2812_Color halfColor;
            halfColor.r = color.r / 2;
            halfColor.g = color.g / 2;
            halfColor.b = color.b / 2;
            WS2812_SetLEDColor(i - 1, halfColor);
        }
        
        if (i - 2 >= 0 && i - 2 < WS2812_NUM_LEDS)
        {
            WS2812_Color quarterColor;
            quarterColor.r = color.r / 4;
            quarterColor.g = color.g / 4;
            quarterColor.b = color.b / 4;
            WS2812_SetLEDColor(i - 2, quarterColor);
        }
        
        // Update LEDs
        WS2812_Show();
        
        // Control speed
        Delay_Ms(speedMs);
    }
    
    // Clear all LEDs when done
    WS2812_Fill(WS2812_CreateColor(0, 0, 0));
    WS2812_Show();
}
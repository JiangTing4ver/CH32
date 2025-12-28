#include "WS2812.h"

// PA8
#define GPIO_Port_GPIO GPIOA
#define GPIO_Pin_GPIO GPIO_Pin_8

// Number of WS2812 LEDs in the strip
#define WS2812_NUM_LEDS 8 


#define WS2812_ONE_HIGH 60   
#define WS2812_ZERO_HIGH 30 
#define WS2812_TIM_ARR 90 



// 全局缓冲区
static WS2812_Color WS2812_ColorBuf[WS2812_NUM_LEDS] = {0};
static uint16_t WS2812_DmaBuf[WS2812_NUM_LEDS * 24] = {0};  // DMA缓冲区（8*24=192bit）

static void TIM1_CH1_DMA_Init(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;
    GPIO_InitTypeDef GPIO_InitStruct;
    DMA_InitTypeDef DMA_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1 | RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_GPIO;
    GPIO_Init(GPIO_Port_GPIO, &GPIO_InitStruct);

    TIM_TimeBaseStructure.TIM_Prescaler = 1 - 1;           
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_Period = WS2812_TIM_ARR - 1;  // 保留你的90周期
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;
    TIM_OCInitStructure.TIM_Pulse = 0;                
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
    TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
    TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
    TIM_OC1Init(TIM1, &TIM_OCInitStructure);

    TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(TIM1, ENABLE);
    TIM_CtrlPWMOutputs(TIM1, ENABLE);

    DMA_DeInit(DMA1_Channel2);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&TIM1->CH1CVR;  // 外设地址：TIM1 CH1CVR
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)WS2812_DmaBuf;    // 内存地址：DMA缓冲区
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;                // 内存→外设
    DMA_InitStructure.DMA_BufferSize = WS2812_NUM_LEDS * 24;          // 缓冲区大小：总比特数
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  // 外设地址不递增
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;           // 内存地址递增
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                     // 普通模式
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel2, &DMA_InitStructure);

    TIM_DMACmd(TIM1, TIM_DMA_CC1, ENABLE);
}

static void WS2812_ConvertToDmaBuf(void)
{
    uint16_t bitIdx = 0;
    uint8_t colorByte, bit;

    for (uint16_t led = 0; led < WS2812_NUM_LEDS; led++)
    {
        // GRB顺序（保留你的逻辑），每个比特对应你的30/60
        colorByte = WS2812_ColorBuf[led].g;
        for (bit = 0; bit < 8; bit++)
        {
            WS2812_DmaBuf[bitIdx++] = (colorByte & 0x80) ? WS2812_ONE_HIGH : WS2812_ZERO_HIGH;
            colorByte <<= 1;
        }

        colorByte = WS2812_ColorBuf[led].r;
        for (bit = 0; bit < 8; bit++)
        {
            WS2812_DmaBuf[bitIdx++] = (colorByte & 0x80) ? WS2812_ONE_HIGH : WS2812_ZERO_HIGH;
            colorByte <<= 1;
        }

        colorByte = WS2812_ColorBuf[led].b;
        for (bit = 0; bit < 8; bit++)
        {
            WS2812_DmaBuf[bitIdx++] = (colorByte & 0x80) ? WS2812_ONE_HIGH : WS2812_ZERO_HIGH;
            colorByte <<= 1;
        }
    }
}

void WS2812_Init(void)
{
    TIM1_CH1_DMA_Init();  
    WS2812_Clear();
}

void WS2812_Show(void)
{
    WS2812_ConvertToDmaBuf();

    DMA_Cmd(DMA1_Channel2, DISABLE);
    DMA_SetCurrDataCounter(DMA1_Channel2, WS2812_NUM_LEDS * 24);
    DMA_Cmd(DMA1_Channel2, ENABLE);

    TIM_Cmd(TIM1, ENABLE);


    while (DMA_GetFlagStatus(DMA1_FLAG_TC2) == RESET);
    DMA_ClearFlag(DMA1_FLAG_TC2);

    TIM_Cmd(TIM1, DISABLE);
    TIM_SetCompare1(TIM1, 0);
    Delay_Us(100);
}

void WS2812_SetLED(uint16_t ledIndex, uint8_t red, uint8_t green, uint8_t blue)
{
    if (ledIndex < WS2812_NUM_LEDS) 
    {
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

void WS2812_Clear(void)
{
    uint16_t i;
    for(i = 0; i < WS2812_NUM_LEDS; i++) 
    {
        WS2812_ColorBuf[i].g = 0;
        WS2812_ColorBuf[i].r = 0;
        WS2812_ColorBuf[i].b = 0;
    }
    WS2812_Show();
}

void WS2812_Fill(WS2812_Color color)
{
    uint16_t i;
    for(i = 0; i < WS2812_NUM_LEDS; i++) 
    {
        WS2812_ColorBuf[i] = color;
    }
    WS2812_Show();
}

void WS2812_SetLEDColor(uint16_t ledIndex, WS2812_Color color)
{
    if (ledIndex < WS2812_NUM_LEDS) 
    {
        WS2812_ColorBuf[ledIndex] = color;
    }
}

// 流水灯效果
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
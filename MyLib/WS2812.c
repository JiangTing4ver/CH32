#include "WS2812.h"

// 亮度值（0-100，对应0%-100%亮度）
uint16_t brightness = 100;

// 全局缓冲区
WS2812_Color WS2812_ColorBuf[WS2812_NUM_LEDS] = {0};
uint16_t WS2812_DmaBuf[WS2812_NUM_LEDS * 24] = {0};  // DMA缓冲区（8*24=192bit）


static uint8_t WS2812_ScaleBrightness(uint8_t colorValue)
{
    // 0-100
    uint16_t safeBrightness = (brightness > 100) ? 100 : (brightness < 0) ? 0 : brightness;
    
    return (uint8_t)((colorValue * safeBrightness + 50) / 100);
}

void TIM1_CH1_DMA_Init(uint16_t psc, uint16_t arr)
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

    TIM_TimeBaseStructure.TIM_Prescaler = psc - 1;           
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_Period = arr - 1;  
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

void WS2812_ConvertToDmaBuf(void)
{
    uint16_t bitIdx = 0;
    uint8_t colorByte, bit;
    uint8_t scaledR, scaledG, scaledB;

    for (uint16_t led = 0; led < WS2812_NUM_LEDS; led++)
    {
        scaledG = WS2812_ScaleBrightness(WS2812_ColorBuf[led].g);
        scaledR = WS2812_ScaleBrightness(WS2812_ColorBuf[led].r);
        scaledB = WS2812_ScaleBrightness(WS2812_ColorBuf[led].b);

        // GRB顺序
        colorByte = scaledG;
        for (bit = 0; bit < 8; bit++)
        {
            WS2812_DmaBuf[bitIdx++] = (colorByte & 0x80) ? WS2812_ONE_HIGH : WS2812_ZERO_HIGH;
            colorByte <<= 1;
        }

        colorByte = scaledR;
        for (bit = 0; bit < 8; bit++)
        {
            WS2812_DmaBuf[bitIdx++] = (colorByte & 0x80) ? WS2812_ONE_HIGH : WS2812_ZERO_HIGH;
            colorByte <<= 1;
        }

        colorByte = scaledB;
        for (bit = 0; bit < 8; bit++)
        {
            WS2812_DmaBuf[bitIdx++] = (colorByte & 0x80) ? WS2812_ONE_HIGH : WS2812_ZERO_HIGH;
            colorByte <<= 1;
        }
    }
}

void WS2812_Init(void)
{
    TIM1_CH1_DMA_Init(1,90);  
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

// 设置全局亮度（0-100）
void WS2812_SetBrightness(uint16_t newBrightness)
{
    brightness = (newBrightness > 100) ? 100 : (newBrightness < 0) ? 0 : newBrightness;
}

uint16_t WS2812_GetBrightness(void)
{
    return brightness;
}

// -----------------------------------------------------------
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
    
    // 初始时清空所有LED
    WS2812_Fill(WS2812_CreateColor(0, 0, 0));
    WS2812_Show();
    Delay_Ms(500);
    
    // 创建流水灯效果
    for (i = 0; i < WS2812_NUM_LEDS + 3; i++)
    {
        // 清空缓冲区
        for (j = 0; j < WS2812_NUM_LEDS; j++)
        {
            WS2812_ColorBuf[j].r = 0;
            WS2812_ColorBuf[j].g = 0;
            WS2812_ColorBuf[j].b = 0;
        }
        
        // 设置当前LED和尾随LED，亮度逐渐降低
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
        
        // 更新LED显示
        WS2812_Show();
        
        // 控制速度
        Delay_Ms(speedMs);
    }
    
    // 结束时清空所有LED
    WS2812_Fill(WS2812_CreateColor(0, 0, 0));
    WS2812_Show();
}

// 双向流水灯效果 - 两束水流向相反方向流动
void WS2812_DoubleWaterFlowEffect(WS2812_Color color, uint16_t speedMs)
{
    uint16_t i, j;
    
    // 初始时清空所有LED
    WS2812_Fill(WS2812_CreateColor(0, 0, 0));
    WS2812_Show();
    Delay_Ms(500);
    
    // 创建双向流水灯效果
    for (i = 0; i < WS2812_NUM_LEDS + 3; i++)
    {
        // 清空缓冲区
        for (j = 0; j < WS2812_NUM_LEDS; j++)
        {
            WS2812_ColorBuf[j].r = 0;
            WS2812_ColorBuf[j].g = 0;
            WS2812_ColorBuf[j].b = 0;
        }
        
        // 正向流动（从左到右）
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
        
        // 反向流动（从右到左）
        uint16_t reverseIndex = WS2812_NUM_LEDS - 1 - i;
        if (reverseIndex >= 0 && reverseIndex < WS2812_NUM_LEDS)
        {
            WS2812_SetLEDColor(reverseIndex, color);
        }
        if (reverseIndex + 1 < WS2812_NUM_LEDS)
        {
            WS2812_Color halfColor;
            halfColor.r = color.r / 2;
            halfColor.g = color.g / 2;
            halfColor.b = color.b / 2;
            WS2812_SetLEDColor(reverseIndex + 1, halfColor);
        }
        
        // 更新LED显示
        WS2812_Show();
        
        // 控制速度
        Delay_Ms(speedMs);
    }
    
    // 结束时清空所有LED
    WS2812_Fill(WS2812_CreateColor(0, 0, 0));
    WS2812_Show();
}

// 彩虹流水灯效果，颜色随位置变化
void WS2812_RainbowWaterFlowEffect(uint16_t speedMs)
{
    uint16_t i, j;
    uint8_t hue = 0;
    
    // 初始时清空所有LED
    WS2812_Fill(WS2812_CreateColor(0, 0, 0));
    WS2812_Show();
    Delay_Ms(500);
    
    // 创建彩虹流水灯效果
    for (i = 0; i < WS2812_NUM_LEDS * 2; i++)
    {
        // 清空缓冲区
        for (j = 0; j < WS2812_NUM_LEDS; j++)
        {
            WS2812_ColorBuf[j].r = 0;
            WS2812_ColorBuf[j].g = 0;
            WS2812_ColorBuf[j].b = 0;
        }
        
        // 计算当前位置的彩虹颜色
        uint8_t currentHue = (hue + i * 10) % 360;
        WS2812_Color rainbowColor;
        
        // 将HSV颜色转换为RGB
        if (currentHue < 60)
        {
            rainbowColor.r = 255;
            rainbowColor.g = (currentHue * 255) / 60;
            rainbowColor.b = 0;
        }
        else if (currentHue < 120)
        {
            rainbowColor.r = (255 - ((currentHue - 60) * 255) / 60);
            rainbowColor.g = 255;
            rainbowColor.b = 0;
        }
        else if (currentHue < 180)
        {
            rainbowColor.r = 0;
            rainbowColor.g = 255;
            rainbowColor.b = ((currentHue - 120) * 255) / 60;
        }
        else if (currentHue < 240)
        {
            rainbowColor.r = 0;
            rainbowColor.g = (255 - ((currentHue - 180) * 255) / 60);
            rainbowColor.b = 255;
        }
        else if (currentHue < 300)
        {
            rainbowColor.r = ((currentHue - 240) * 255) / 60;
            rainbowColor.g = 0;
            rainbowColor.b = 255;
        }
        else
        {
            rainbowColor.r = 255;
            rainbowColor.g = 0;
            rainbowColor.b = (255 - ((currentHue - 300) * 255) / 60);
        }
        
        // 设置当前LED和尾随LED，亮度逐渐降低
        uint16_t ledPos = i % (WS2812_NUM_LEDS + 3);
        if (ledPos < WS2812_NUM_LEDS)
        {
            WS2812_SetLEDColor(ledPos, rainbowColor);
        }
        if (ledPos - 1 >= 0 && ledPos - 1 < WS2812_NUM_LEDS)
        {
            WS2812_Color halfColor;
            halfColor.r = rainbowColor.r / 2;
            halfColor.g = rainbowColor.g / 2;
            halfColor.b = rainbowColor.b / 2;
            WS2812_SetLEDColor(ledPos - 1, halfColor);
        }
        if (ledPos - 2 >= 0 && ledPos - 2 < WS2812_NUM_LEDS)
        {
            WS2812_Color quarterColor;
            quarterColor.r = rainbowColor.r / 4;
            quarterColor.g = rainbowColor.g / 4;
            quarterColor.b = rainbowColor.b / 4;
            WS2812_SetLEDColor(ledPos - 2, quarterColor);
        }
        
        // 更新LED显示
        WS2812_Show();
        
        // 控制速度
        Delay_Ms(speedMs);
    }
    
    // 结束时清空所有LED
    WS2812_Fill(WS2812_CreateColor(0, 0, 0));
    WS2812_Show();
}

// 呼吸流水灯效果，亮度呈脉冲变化
void WS2812_BreathingWaterFlowEffect(WS2812_Color color, uint16_t speedMs)
{
    uint16_t i, j;
    uint8_t breathPhase = 0;
    
    // 初始时清空所有LED
    WS2812_Fill(WS2812_CreateColor(0, 0, 0));
    WS2812_Show();
    Delay_Ms(500);
    
    // 创建呼吸流水灯效果
    for (i = 0; i < WS2812_NUM_LEDS + 10; i++)
    {
        // 清空缓冲区
        for (j = 0; j < WS2812_NUM_LEDS; j++)
        {
            WS2812_ColorBuf[j].r = 0;
            WS2812_ColorBuf[j].g = 0;
            WS2812_ColorBuf[j].b = 0;
        }
        
        // 计算呼吸亮度（类似正弦波）
        uint8_t brightness;
        if (breathPhase < 128)
        {
            brightness = (breathPhase * 2);
        }
        else
        {
            brightness = 255 - ((breathPhase - 128) * 2);
        }
        breathPhase = (breathPhase + 5) % 256;
        
        // 将亮度应用到颜色
        WS2812_Color breathingColor;
        breathingColor.r = (color.r * brightness) / 255;
        breathingColor.g = (color.g * brightness) / 255;
        breathingColor.b = (color.b * brightness) / 255;
        
        // 设置当前LED和尾随LED，亮度逐渐降低
        if (i < WS2812_NUM_LEDS)
        {
            WS2812_SetLEDColor(i, breathingColor);
        }
        if (i - 1 >= 0 && i - 1 < WS2812_NUM_LEDS)
        {
            WS2812_Color halfColor;
            halfColor.r = breathingColor.r / 2;
            halfColor.g = breathingColor.g / 2;
            halfColor.b = breathingColor.b / 2;
            WS2812_SetLEDColor(i - 1, halfColor);
        }
        if (i - 2 >= 0 && i - 2 < WS2812_NUM_LEDS)
        {
            WS2812_Color quarterColor;
            quarterColor.r = breathingColor.r / 4;
            quarterColor.g = breathingColor.g / 4;
            quarterColor.b = breathingColor.b / 4;
            WS2812_SetLEDColor(i - 2, quarterColor);
        }
        
        // 更新LED显示
        WS2812_Show();
        
        // 控制速度
        Delay_Ms(speedMs);
    }
    
    // 结束时清空所有LED
    WS2812_Fill(WS2812_CreateColor(0, 0, 0));
    WS2812_Show();
}

void Run_FlowRGB(uint16_t speedMs)
{
    // 循环显示不同颜色的流水灯效果
    WS2812_WaterFlowEffect(WS2812_CreateColor(255, 0, 0), speedMs); // 红色流水灯
    WS2812_WaterFlowEffect(WS2812_CreateColor(0, 255, 0), speedMs); // 绿色流水灯
    WS2812_WaterFlowEffect(WS2812_CreateColor(0, 0, 255), speedMs); // 蓝色流水灯
    WS2812_WaterFlowEffect(WS2812_CreateColor(255, 255, 0), speedMs); // 黄色流水灯
    WS2812_WaterFlowEffect(WS2812_CreateColor(255, 0, 255), speedMs); // 紫色流水灯
    WS2812_WaterFlowEffect(WS2812_CreateColor(0, 255, 255), speedMs); // 青色流水灯
    WS2812_WaterFlowEffect(WS2812_CreateColor(255, 255, 255), speedMs); // 白色流水灯
}

// 追逐效果 - 两种颜色相互追逐
void WS2812_ChaseEffect(WS2812_Color color1, WS2812_Color color2, uint16_t speedMs)
{
    uint16_t i, j;
    
    // 初始时清空所有LED
    WS2812_Fill(WS2812_CreateColor(0, 0, 0));
    WS2812_Show();
    Delay_Ms(500);
    
    // 创建追逐效果
    for (i = 0; i < WS2812_NUM_LEDS * 2; i++)
    {
        // 清空缓冲区
        for (j = 0; j < WS2812_NUM_LEDS; j++)
        {
            WS2812_ColorBuf[j].r = 0;
            WS2812_ColorBuf[j].g = 0;
            WS2812_ColorBuf[j].b = 0;
        }
        
        // 设置两种颜色的LED相互追逐
        for (j = 0; j < WS2812_NUM_LEDS; j++)
        {
            if ((j + i) % 2 == 0)
            {
                WS2812_SetLEDColor(j, color1);
            }
            else
            {
                WS2812_SetLEDColor(j, color2);
            }
        }
        
        // 更新LED显示
        WS2812_Show();
        
        // 控制速度
        Delay_Ms(speedMs);
    }
    
    // 结束时清空所有LED
    WS2812_Fill(WS2812_CreateColor(0, 0, 0));
    WS2812_Show();
}

// 弹跳效果 - LED来回弹跳
void WS2812_BounceEffect(WS2812_Color color, uint16_t speedMs)
{
    uint16_t i, j;
    int8_t direction = 1;
    uint16_t position = 0;
    
    // 初始时清空所有LED
    WS2812_Fill(WS2812_CreateColor(0, 0, 0));
    WS2812_Show();
    Delay_Ms(500);
    
    // 创建多轮弹跳效果
    for (i = 0; i < WS2812_NUM_LEDS * 4; i++)
    {
        // 清空缓冲区
        for (j = 0; j < WS2812_NUM_LEDS; j++)
        {
            WS2812_ColorBuf[j].r = 0;
            WS2812_ColorBuf[j].g = 0;
            WS2812_ColorBuf[j].b = 0;
        }
        
        // 设置当前位置和拖尾效果
        if (position < WS2812_NUM_LEDS)
        {
            WS2812_SetLEDColor(position, color);
        }
        if (position - 1 >= 0 && position - 1 < WS2812_NUM_LEDS)
        {
            WS2812_Color halfColor;
            halfColor.r = color.r / 2;
            halfColor.g = color.g / 2;
            halfColor.b = color.b / 2;
            WS2812_SetLEDColor(position - 1, halfColor);
        }
        if (position - 2 >= 0 && position - 2 < WS2812_NUM_LEDS)
        {
            WS2812_Color quarterColor;
            quarterColor.r = color.r / 4;
            quarterColor.g = color.g / 4;
            quarterColor.b = color.b / 4;
            WS2812_SetLEDColor(position - 2, quarterColor);
        }
        
        // 更新LED显示
        WS2812_Show();
        
        // 移动位置并在需要时改变方向
        position += direction;
        if (position >= WS2812_NUM_LEDS - 1 || position <= 0)
        {
            direction *= -1;
        }
        
        // 控制速度
        Delay_Ms(speedMs);
    }
    
    // 结束时清空所有LED
    WS2812_Fill(WS2812_CreateColor(0, 0, 0));
    WS2812_Show();
}

/*
 * @brief 呼吸灯效果（极简版）
 * @param ledIndex 要控制的LED索引，0到WS2812_NUM_LEDS-1为单个LED，WS2812_NUM_LEDS为所有LED
 * @param r 红色分量（0-255）
 * @param g 绿色分量（0-255）
 * @param b 蓝色分量（0-255）
 * @param delayMs 每次亮度变化的延时（毫秒，建议10-50，值越小呼吸越快）
 * @note 1. 亮度变化步长固定为1（保证呼吸流畅）
 *       2. 函数会无限循环执行呼吸效果（0→100→0亮度循环）
 *       3. 控制单个/所有LED，颜色固定，仅亮度循环变化
 */
void WS2812_BreathingLight(uint16_t ledIndex, uint8_t r, uint8_t g, uint8_t b, uint16_t delayMs)
{
    if (delayMs == 0) delayMs = 1;

    if (ledIndex < WS2812_NUM_LEDS)
    {
        // 控制单个灯珠
        WS2812_SetLED(ledIndex, r, g, b);
    }
    else if (ledIndex == WS2812_NUM_LEDS)
    {
        // 控制所有灯珠
        WS2812_Fill(WS2812_CreateColor(r, g, b));
    }
    else
    {
        return;
    }

    // 渐亮阶段：亮度从0增加到100
    for (brightness = 0; brightness <= 100; brightness++)
    {
        WS2812_Show();  // 刷新当前亮度
        Delay_Ms(delayMs);
    }

    // 渐暗阶段：亮度从100降低到0
    for (brightness = 100; brightness >= 0; brightness--)
    {
        WS2812_Show();  // 刷新当前亮度
        Delay_Ms(delayMs);
    }
    WS2812_Clear();
}
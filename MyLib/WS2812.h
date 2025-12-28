#ifndef __WS2812_H__
#define __WS2812_H__

#include "ch32f10x_gpio.h"
#include "ch32f10x_rcc.h"
#include "ch32f10x_tim.h"

void TIM1_CH1_Init(uint16_t psc, uint16_t arr, uint16_t ccr);
void WS2812_Pin_init();
void WS2812_Init(void);

// 内部使用的发送函数
void send_1(void);
void send_0(void);

// WS2812的颜色结构（GRB格式）
typedef struct {
    uint8_t g;
    uint8_t r;
    uint8_t b;
} WS2812_Color;

// 公共API函数
void WS2812_SendColor(WS2812_Color color);
void WS2812_SendByte(uint8_t byte);
void WS2812_Show(void);
void WS2812_SetLED(uint16_t ledIndex, uint8_t red, uint8_t green, uint8_t blue);
void WS2812_SetLEDColor(uint16_t ledIndex, WS2812_Color color);
void WS2812_Fill(WS2812_Color color);
WS2812_Color WS2812_CreateColor(uint8_t red, uint8_t green, uint8_t blue);
void WS2812_Clear(void);
void WS2812_WaterFlowEffect(WS2812_Color color, uint16_t speedMs);
void Run_FlowRGB(uint16_t speedMs);
// 其他高级效果
void WS2812_DoubleWaterFlowEffect(WS2812_Color color, uint16_t speedMs);
void WS2812_RainbowWaterFlowEffect(uint16_t speedMs);
void WS2812_BreathingWaterFlowEffect(WS2812_Color color, uint16_t speedMs);
void WS2812_ChaseEffect(WS2812_Color color1, WS2812_Color color2, uint16_t speedMs);
void WS2812_BounceEffect(WS2812_Color color, uint16_t speedMs);

#endif // __WS2812_H__
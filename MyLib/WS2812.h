#ifndef __WS2812_H__
#define __WS2812_H__

#include "ch32f10x_gpio.h"
#include "ch32f10x_rcc.h"
#include "ch32f10x_tim.h"

void TIM1_CH1_Init(uint16_t psc, uint16_t arr, uint16_t ccr);
void WS2812_Pin_init();
void WS2812_Init(void);

// Send functions for internal use
void send_1(void);
void send_0(void);

// Color structure for WS2812 (GRB format)
typedef struct {
    uint8_t g;
    uint8_t r;
    uint8_t b;
} WS2812_Color;

// Public API functions
void WS2812_SendColor(WS2812_Color color);
void WS2812_Show(void);
void WS2812_SetLED(uint16_t ledIndex, uint8_t red, uint8_t green, uint8_t blue);
void WS2812_Fill(WS2812_Color color);
WS2812_Color WS2812_CreateColor(uint8_t red, uint8_t green, uint8_t blue);
void WS2812_Clear(void);

#endif // __WS2812_H__
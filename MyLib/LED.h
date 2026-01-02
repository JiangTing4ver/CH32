#ifndef __LED_H__
#define __LED_H__

#include "GPIO_contrl.h"
#include "delay.h"


void LED_Init(void);
void LED_On(void);
void LED_Off(void);
void LED_blink(uint16_t delay_ms);
#endif

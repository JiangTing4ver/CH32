#ifndef __LED_H__
#define __LED_H__

#include "GPIO_contrl.h"
#include "delay.h"

#define DELAY_MS1 1000
#define DELAY_MS2 100

void LED_Init(void);
void LED_On(void);
void LED_Off(void);
void LED_blink(void);
#endif

#ifndef __MH_SERSOR_H__
#define __MH_SERSOR_H__

#include "ch32f10x_gpio.h"
#include "LED.h"
#define RCC_Periph_MH_Sersor RCC_APB2Periph_GPIOA
#define MH_Sersor_Port GPIOA
#define MH_Sersor_Pin GPIO_Pin_0

void MH_Sersor_Init();
void Note();

#endif
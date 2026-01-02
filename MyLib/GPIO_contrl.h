#ifndef __GPIO_CONTRL_H__
#define __GPIO_CONTRL_H__

#include "ch32f10x.h"
// 宏定义
#define RCC_Periph_GPIO RCC_APB2Periph_GPIOC
#define GPIO_Port_CONTROL GPIOC
#define GPIO_Pin_CONTROL GPIO_Pin_13

// 函数声明
void GPIO_OUT_SetInit(uint32_t RCC_Periph_GPIOx, GPIO_TypeDef* GPIO_Port, uint16_t GPIO_Pin);
void GPIO_IN_SetInit(uint32_t RCC_Periph_GPIOx, GPIO_TypeDef* GPIO_Port, uint16_t GPIO_Pin);
#endif

/********************************** (C) COPYRIGHT  *******************************
 * File Name          : delay.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2019/10/15
 * Description        : Delay functions implementation.
 *********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "delay.h"
#include "ch32f10x.h"

static uint32_t fac_us = 0; // Microsecond delay multiplier
static uint32_t fac_ms = 0; // Millisecond delay multiplier

/*********************************************************************
 * @fn      Delay_Init
 * @brief   Initializes delay functions.
 * @return  none
 ********************************************************************/
void Delay_Init(void)
{
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8); // Configure SysTick clock source as HCLK/8
    fac_us = SystemCoreClock / 8000000; // Calculate microsecond delay multiplier
    fac_ms = fac_us * 1000; // Calculate millisecond delay multiplier
}

/*********************************************************************
 * @fn      Delay_Us
 * @brief   Microsecond delay function.
 * @param   n - Microseconds to delay.
 * @return  none
 ********************************************************************/
void Delay_Us(u32 n)
{
    u32 temp;
    SysTick->LOAD = n * fac_us; // Set reload value
    SysTick->VAL = 0x00; // Clear current value register
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk; // Start counting
    
    do {
        temp = SysTick->CTRL;
    } while ((temp & 0x01) && !(temp & (1 << 16))); // Wait for timeout
    
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk; // Disable SysTick
    SysTick->VAL = 0x00; // Clear current value register
}

/*********************************************************************
 * @fn      Delay_Ms
 * @brief   Millisecond delay function.
 * @param   n - Milliseconds to delay.
 * @return  none
 ********************************************************************/
void Delay_Ms(u32 n)
{
    u32 temp;
    SysTick->LOAD = n * fac_ms; // Set reload value
    SysTick->VAL = 0x00; // Clear current value register
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk; // Start counting
    
    do {
        temp = SysTick->CTRL;
    } while ((temp & 0x01) && !(temp & (1 << 16))); // Wait for timeout
    
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk; // Disable SysTick
    SysTick->VAL = 0x00; // Clear current value register
}

#include "LED.h"
#include "ch32f10x_gpio.h"

void LED_Init(void)
{
    GPIO_OUT_SetInit(RCC_Periph_GPIO, GPIO_Port_CONTROL, GPIO_Pin_CONTROL);
    GPIO_ResetBits(GPIOC, GPIO_Pin_13);
}

void LED_On(void)
{
    GPIO_SetBits(GPIOC, GPIO_Pin_13);
}

void LED_Off(void)
{
    GPIO_ResetBits(GPIOC, GPIO_Pin_13);
}

void LED_blink(uint16_t delay_ms)
{
    GPIO_SetBits(GPIOC, GPIO_Pin_13);
    Delay_Ms(delay_ms); 
    GPIO_ResetBits(GPIOC, GPIO_Pin_13);
    Delay_Ms(delay_ms);
}
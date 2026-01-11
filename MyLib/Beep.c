#include "Beep.h"

#define BEEP_GPIO_Port GPIOA
#define BEEP_GPIO_Pin GPIO_Pin_15


void BEEP_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin = BEEP_GPIO_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(BEEP_GPIO_Port, &GPIO_InitStructure);
}

void BEEP_On(void)
{
    GPIO_ResetBits(BEEP_GPIO_Port, BEEP_GPIO_Pin);  // 低电平有效
}

void BEEP_Off(void)
{
    GPIO_SetBits(BEEP_GPIO_Port, BEEP_GPIO_Pin);
}
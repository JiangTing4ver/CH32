#include "MH_Sersor.h"

void MH_Sersor_Init()
{
    GPIO_IN_SetInit(RCC_Periph_MH_Sersor, MH_Sersor_Port, MH_Sersor_Pin);
}
void Note()
{
    LED_Init();
    if(GPIO_ReadInputDataBit(MH_Sersor_Port, MH_Sersor_Pin) == 1)
    {
        LED_On();
    }
    else if(GPIO_ReadInputDataBit(MH_Sersor_Port, MH_Sersor_Pin) == 0)
    {
        LED_Off();
    }
}
#include "MH_Sersor.h"
/**
 * @file MH_Sersor.c
 * @author 4verJiangting
 * @brief 获取红外模块的状态
 * @version 0.1
 * @date 2025-12-20
 */
/**
 * @brief 初始化传感器
 * @param RCC_Periph_MH_Sersor 传感器端口时钟
 * @param MH_Sersor_Port 传感器端口
 * @param MH_Sersor_Pin 传感器引脚
 */
void MH_Sersor_Init()
{
    GPIO_IN_SetInit(RCC_Periph_MH_Sersor, MH_Sersor_Port, MH_Sersor_Pin);
}
/**
 * @brief 提示传感器状态
 */
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
#ifndef __MYIIC_H__
#define __MYIIC_H__

#include "ch32f10x_gpio.h"
#include "ch32f10x_rcc.h"
#include "ch32f10x_i2c.h"

// I2C端口定义
#define MYIIC_SCL_PORT        GPIOB
#define MYIIC_SCL_PIN         GPIO_Pin_6
#define MYIIC_SDA_PORT        GPIOB
#define MYIIC_SDA_PIN         GPIO_Pin_7

// I2C设备定义
#define MYIIC_I2C             I2C1
#define MYIIC_I2C_CLK         RCC_APB1Periph_I2C1
#define MYIIC_I2C_GPIO_CLK    RCC_APB2Periph_GPIOB

// 函数声明
void MyIIC_Init(void);
u8 MyIIC_ReadByte(u8 addr, u8 reg);
u8 MyIIC_WriteByte(u8 addr, u8 reg, u8 data);
u8 MyIIC_ReadBuffer(u8 addr, u8 reg, u8 *buf, u8 len);
u8 MyIIC_WriteBuffer(u8 addr, u8 reg, u8 *buf, u8 len);

#endif // __MYIIC_H__
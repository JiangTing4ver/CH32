#ifndef __MYIIC_H__
#define __MYIIC_H__

#include "ch32f10x_gpio.h"
#include "ch32f10x_rcc.h"
#include "ch32f10x_i2c.h"

/* 全局定义 */
#define Size        6       // 每次发送的数据长度
#define TxAddress   0x02    // 主机自身地址（可根据需求修改）
#define SlaveAddr   0x02    // 目标从机地址（需和从机地址匹配）
#define I2C_SPEED   80000   // I2C通信速率（80KHz）

/* 软件I2C引脚定义 */
#define I2C_GPIO_PORT GPIOB
#define I2C_SCL_PIN   GPIO_Pin_6
#define I2C_SDA_PIN   GPIO_Pin_7

/* 全局变量 */
extern uint8_t TxData[Size];  // 待发送的数据

void IIC_Init( u32 bound, u16 address );
uint8_t I2C_Host_SendData(u8 slave_addr, u8 *data, u8 len);
void I2C_Soft_Delay(void);
void I2C_Soft_Init(void);
void I2C_Soft_Start(void);
void I2C_Soft_Stop(void);
uint8_t I2C_Soft_SendByte(uint8_t dat);
uint8_t I2C_Soft_SendData(uint8_t slave_addr, uint8_t *data, uint8_t len);

#endif // __MYIIC_H__
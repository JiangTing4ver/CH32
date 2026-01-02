#include "myiic.h"

// /* 全局定义 */
// #define Size        6       // 每次发送的数据长度
// #define TxAddress   0x02    // 主机自身地址（可根据需求修改）
// #define SlaveAddr   0x02    // 目标从机地址（需和从机地址匹配）
// #define I2C_SPEED   80000   // I2C通信速率（80KHz）

/* 全局变量 */
uint8_t TxData[Size] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06};  // 待发送的数据

/*********************************************************************
 * @fn      IIC_Init
 * @brief   初始化I2C1外设（主机模式）
 * @param   bound: 通信速率  address: 主机自身地址
 * @return  none
 */
void IIC_Init(u32 bound, u16 address)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    I2C_InitTypeDef I2C_InitTSturcture = {0};

    // 使能时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

    // 配置PB6（SCL）为复用开漏模式
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;  // I2C必须用开漏模式
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // 配置PB7（SDA）为复用开漏模式
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // 配置I2C核心参数
    I2C_InitTSturcture.I2C_ClockSpeed = bound;
    I2C_InitTSturcture.I2C_Mode = I2C_Mode_I2C;
    I2C_InitTSturcture.I2C_DutyCycle = I2C_DutyCycle_16_9;
    I2C_InitTSturcture.I2C_OwnAddress1 = address;
    I2C_InitTSturcture.I2C_Ack = I2C_Ack_Enable;
    I2C_InitTSturcture.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_Init(I2C1, &I2C_InitTSturcture);

    // 使能I2C1
    I2C_Cmd(I2C1, ENABLE);
}

/*********************************************************************
 * @fn      I2C_Host_SendData
 * @brief   主机发送一组数据到从机
 * @param   slave_addr: 从机地址  data: 数据缓冲区  len: 数据长度
 * @return  none
 */
void I2C_Host_SendData(u8 slave_addr, u8 *data, u8 len)
{
    u8 i = 0;
    while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY) != RESET);
    I2C_GenerateSTART(I2C1, ENABLE);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
    I2C_Send7bitAddress(I2C1, slave_addr, I2C_Direction_Transmitter);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

    for(i = 0; i < len; i++)
    {
        while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTING));
        I2C_SendData(I2C1, data[i]);
    }

    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    I2C_GenerateSTOP(I2C1, ENABLE);
}
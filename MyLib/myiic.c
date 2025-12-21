#include "myiic.h"

void MyIIC_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    I2C_InitTypeDef I2C_InitStructure;

    // 使能GPIO时钟
    RCC_APB2PeriphClockCmd(MYIIC_I2C_GPIO_CLK, ENABLE);
    // 使能I2C时钟
    RCC_APB1PeriphClockCmd(MYIIC_I2C_CLK, ENABLE);

    // 配置GPIO引脚为复用推挽输出
    GPIO_InitStructure.GPIO_Pin = MYIIC_SCL_PIN | MYIIC_SDA_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(MYIIC_SCL_PORT, &GPIO_InitStructure);

    // 配置I2C
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1 = 0x00;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_ClockSpeed = 100000; // 100kHz
    I2C_Init(MYIIC_I2C, &I2C_InitStructure);

    // 使能I2C
    I2C_Cmd(MYIIC_I2C, ENABLE);
}

u8 MyIIC_WriteByte(u8 addr, u8 reg, u8 data)
{
    // 等待I2C总线空闲
    while (I2C_GetFlagStatus(MYIIC_I2C, I2C_FLAG_BUSY));

    // 发送起始信号
    I2C_GenerateSTART(MYIIC_I2C, ENABLE);
    while (!I2C_CheckEvent(MYIIC_I2C, I2C_EVENT_MASTER_MODE_SELECT));

    // 发送设备地址（写模式）
    I2C_Send7bitAddress(MYIIC_I2C, addr << 1, I2C_Direction_Transmitter);
    while (!I2C_CheckEvent(MYIIC_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

    // 发送寄存器地址
    I2C_SendData(MYIIC_I2C, reg);
    while (!I2C_CheckEvent(MYIIC_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    // 发送数据
    I2C_SendData(MYIIC_I2C, data);
    while (!I2C_CheckEvent(MYIIC_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    // 发送停止信号
    I2C_GenerateSTOP(MYIIC_I2C, ENABLE);

    return 0;
}

u8 MyIIC_ReadByte(u8 addr, u8 reg)
{
    u8 data;

    // 等待I2C总线空闲
    while (I2C_GetFlagStatus(MYIIC_I2C, I2C_FLAG_BUSY));

    // 发送起始信号
    I2C_GenerateSTART(MYIIC_I2C, ENABLE);
    while (!I2C_CheckEvent(MYIIC_I2C, I2C_EVENT_MASTER_MODE_SELECT));

    // 发送设备地址（写模式）
    I2C_Send7bitAddress(MYIIC_I2C, addr << 1, I2C_Direction_Transmitter);
    while (!I2C_CheckEvent(MYIIC_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

    // 发送寄存器地址
    I2C_SendData(MYIIC_I2C, reg);
    while (!I2C_CheckEvent(MYIIC_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    // 重新发送起始信号
    I2C_GenerateSTART(MYIIC_I2C, ENABLE);
    while (!I2C_CheckEvent(MYIIC_I2C, I2C_EVENT_MASTER_MODE_SELECT));

    // 发送设备地址（读模式）
    I2C_Send7bitAddress(MYIIC_I2C, addr << 1, I2C_Direction_Receiver);
    while (!I2C_CheckEvent(MYIIC_I2C, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));

    // 关闭Ack
    I2C_AcknowledgeConfig(MYIIC_I2C, DISABLE);
    // 发送停止信号
    I2C_GenerateSTOP(MYIIC_I2C, ENABLE);

    // 接收数据
    while (!I2C_CheckEvent(MYIIC_I2C, I2C_EVENT_MASTER_BYTE_RECEIVED));
    data = I2C_ReceiveData(MYIIC_I2C);

    // 重新使能Ack
    I2C_AcknowledgeConfig(MYIIC_I2C, ENABLE);

    return data;
}

u8 MyIIC_ReadBuffer(u8 addr, u8 reg, u8 *buf, u8 len)
{
    u8 i;

    if (len == 0) return 1;

    // 等待I2C总线空闲
    while (I2C_GetFlagStatus(MYIIC_I2C, I2C_FLAG_BUSY));

    // 发送起始信号
    I2C_GenerateSTART(MYIIC_I2C, ENABLE);
    while (!I2C_CheckEvent(MYIIC_I2C, I2C_EVENT_MASTER_MODE_SELECT));

    // 发送设备地址（写模式）
    I2C_Send7bitAddress(MYIIC_I2C, addr << 1, I2C_Direction_Transmitter);
    while (!I2C_CheckEvent(MYIIC_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

    // 发送寄存器地址
    I2C_SendData(MYIIC_I2C, reg);
    while (!I2C_CheckEvent(MYIIC_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    // 重新发送起始信号
    I2C_GenerateSTART(MYIIC_I2C, ENABLE);
    while (!I2C_CheckEvent(MYIIC_I2C, I2C_EVENT_MASTER_MODE_SELECT));

    // 发送设备地址（读模式）
    I2C_Send7bitAddress(MYIIC_I2C, addr << 1, I2C_Direction_Receiver);
    while (!I2C_CheckEvent(MYIIC_I2C, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));

    // 读取数据
    for (i = 0; i < len; i++)
    {
        if (i == len - 1) // 最后一个字节
        {
            // 关闭Ack
            I2C_AcknowledgeConfig(MYIIC_I2C, DISABLE);
            // 发送停止信号
            I2C_GenerateSTOP(MYIIC_I2C, ENABLE);
        }

        // 等待数据接收完成
        while (!I2C_CheckEvent(MYIIC_I2C, I2C_EVENT_MASTER_BYTE_RECEIVED));
        buf[i] = I2C_ReceiveData(MYIIC_I2C);
    }

    // 重新使能Ack
    I2C_AcknowledgeConfig(MYIIC_I2C, ENABLE);

    return 0;
}

u8 MyIIC_WriteBuffer(u8 addr, u8 reg, u8 *buf, u8 len)
{
    u8 i;

    if (len == 0) return 1;

    // 等待I2C总线空闲
    while (I2C_GetFlagStatus(MYIIC_I2C, I2C_FLAG_BUSY));

    // 发送起始信号
    I2C_GenerateSTART(MYIIC_I2C, ENABLE);
    while (!I2C_CheckEvent(MYIIC_I2C, I2C_EVENT_MASTER_MODE_SELECT));

    // 发送设备地址（写模式）
    I2C_Send7bitAddress(MYIIC_I2C, addr << 1, I2C_Direction_Transmitter);
    while (!I2C_CheckEvent(MYIIC_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

    // 发送寄存器地址
    I2C_SendData(MYIIC_I2C, reg);
    while (!I2C_CheckEvent(MYIIC_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    // 发送数据
    for (i = 0; i < len; i++)
    {
        I2C_SendData(MYIIC_I2C, buf[i]);
        while (!I2C_CheckEvent(MYIIC_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    }

    // 发送停止信号
    I2C_GenerateSTOP(MYIIC_I2C, ENABLE);

    return 0;
}
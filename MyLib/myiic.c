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
 * @param   slave_addr: 从机地址（7位地址）  data: 数据缓冲区  len: 数据长度
 * @return  uint8_t: 0表示成功，1表示失败
 */
uint8_t I2C_Host_SendData(u8 slave_addr, u8 *data, u8 len)
{
    u8 i = 0;
    uint32_t timeout = 0;
    const uint32_t MAX_TIMEOUT = 100000;

    // 等待总线空闲
    timeout = 0;
    while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY) != RESET)
    {
        if(timeout++ > MAX_TIMEOUT) return 1;
    }

    // 发送起始信号
    I2C_GenerateSTART(I2C1, ENABLE);
    
    // 等待起始信号发送完成
    timeout = 0;
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
    {
        if(timeout++ > MAX_TIMEOUT) return 1;
    }

    // 发送从机地址（7位地址左移1位，方向为发送）
    I2C_Send7bitAddress(I2C1, slave_addr << 1, I2C_Direction_Transmitter);
    
    // 等待地址发送完成并收到ACK
    timeout = 0;
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    {
        // 检查是否收到NACK
        if(I2C_GetFlagStatus(I2C1, I2C_FLAG_AF) != RESET)
        {
            I2C_ClearFlag(I2C1, I2C_FLAG_AF);
            I2C_GenerateSTOP(I2C1, ENABLE);
            return 1;
        }
        if(timeout++ > MAX_TIMEOUT)
        {
            I2C_GenerateSTOP(I2C1, ENABLE);
            return 1;
        }
    }

    // 发送数据
    for(i = 0; i < len; i++)
    {
        timeout = 0;
        while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTING))
        {
            if(timeout++ > MAX_TIMEOUT)
            {
                I2C_GenerateSTOP(I2C1, ENABLE);
                return 1;
            }
        }
        I2C_SendData(I2C1, data[i]);
    }

    // 等待最后一个字节发送完成
    timeout = 0;
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
        if(timeout++ > MAX_TIMEOUT)
        {
            I2C_GenerateSTOP(I2C1, ENABLE);
            return 1;
        }
    }

    // 发送停止信号
    I2C_GenerateSTOP(I2C1, ENABLE);
    return 0;
}

// ------软件iic-----------
void I2C_Soft_Delay(void)
{
    Delay_Us(3);  // 5us延时，速率≈100KHz；改10us=50KHz，改3us=150KHz
}

void I2C_Soft_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    // 使能GPIOB时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    // 配置SCL/SDA为推挽输出（关键：推挽能主动输出高低电平，无需上拉）
    GPIO_InitStructure.GPIO_Pin = I2C_SCL_PIN | I2C_SDA_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  // 推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(I2C_GPIO_PORT, &GPIO_InitStructure);

    // 初始电平：SCL、SDA都置高（I2C空闲状态）
    GPIO_SetBits(I2C_GPIO_PORT, I2C_SCL_PIN | I2C_SDA_PIN);
}

/*********************************************************************
 * @fn      I2C_Soft_Start
 * @brief   软件I2C起始信号（SDA先拉低，SCL保持高→拉低）
 */
void I2C_Soft_Start(void)
{
    // I2C起始位特征：SCL高时，SDA从高→低
    GPIO_SetBits(I2C_GPIO_PORT, I2C_SDA_PIN);
    I2C_Soft_Delay();
    GPIO_ResetBits(I2C_GPIO_PORT, I2C_SDA_PIN);
    I2C_Soft_Delay();
    GPIO_ResetBits(I2C_GPIO_PORT, I2C_SCL_PIN);
    I2C_Soft_Delay();
}

/*********************************************************************
 * @fn      I2C_Soft_Stop
 * @brief   软件I2C停止信号（SDA先拉低，SCL拉高后SDA→高）
 */
void I2C_Soft_Stop(void)
{
    // I2C停止位特征：SCL高时，SDA从低→高
    GPIO_ResetBits(I2C_GPIO_PORT, I2C_SDA_PIN);
    I2C_Soft_Delay();
    GPIO_SetBits(I2C_GPIO_PORT, I2C_SCL_PIN);
    I2C_Soft_Delay();
    GPIO_SetBits(I2C_GPIO_PORT, I2C_SDA_PIN);
    I2C_Soft_Delay();
}

/*********************************************************************
 * @fn      I2C_Soft_SendByte
 * @brief   软件I2C发送1字节数据（高位先送）
 * @param   dat: 要发送的字节
 * @return  1:模拟从机应答（测试用），0:无应答
 */
uint8_t I2C_Soft_SendByte(uint8_t dat)
{
    uint8_t i = 0;

    // 逐位发送8位数据（高位在前）
    for(i = 0; i < 8; i++)
    {
        // 输出当前位（SCL低电平时更新SDA）
        if(dat & 0x80)  // 发送最高位
            GPIO_SetBits(I2C_GPIO_PORT, I2C_SDA_PIN);
        else
            GPIO_ResetBits(I2C_GPIO_PORT, I2C_SDA_PIN);
        
        dat <<= 1;      // 左移，准备下一位
        I2C_Soft_Delay();
        
        // SCL拉高，让从机采样当前位
        GPIO_SetBits(I2C_GPIO_PORT, I2C_SCL_PIN);
        I2C_Soft_Delay();
        
        // SCL拉低，准备发送下一位
        GPIO_ResetBits(I2C_GPIO_PORT, I2C_SCL_PIN);
        I2C_Soft_Delay();
    }

    // 模拟从机应答（测试波形无需真实应答，直接返回1）
    return 1;
}

/*********************************************************************
 * @fn      I2C_Soft_SendData
 * @brief   软件I2C主机发送一组数据到从机（和硬件I2C接口一致，方便切换）
 * @param   slave_addr: 从机地址（7位）  data: 数据缓冲区  len: 数据长度
 * @return  0:成功，1:失败（测试用固定返回0）
 */
uint8_t I2C_Soft_SendData(uint8_t slave_addr, uint8_t *data, uint8_t len)
{
    uint8_t i = 0;

    // 1. 发送起始信号
    I2C_Soft_Start();
    
    // 2. 发送从机地址+写位（7位地址左移1位 + 0=写）
    I2C_Soft_SendByte((slave_addr << 1) | 0);
    
    // 3. 循环发送数据
    for(i = 0; i < len; i++)
    {
        I2C_Soft_SendByte(data[i]);
    }
    
    // 4. 发送停止信号
    I2C_Soft_Stop();

    return 0;  // 软件I2C测试用，固定返回成功
}

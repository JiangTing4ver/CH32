#include "gy520.h"

// 陀螺仪和加速度计量程参数
static float gyro_scale = 131.0f;  // 默认±250°/s，131 LSB/(°/s)
static float accel_scale = 16384.0f; // 默认±2g，16384 LSB/g

u8 GY520_Init(void)
{
    u8 id;
    
    // 唤醒MPU-6050
    MyIIC_WriteByte(GY520_ADDR, GY520_PWR_MGMT_1, 0x00);
    
    // 检查设备ID
    id = MyIIC_ReadByte(GY520_ADDR, GY520_WHO_AM_I);
    if (id != GY520_ADDR) return 1;
    
    // 设置采样率分频器 (采样率=陀螺仪输出频率/(1+SMPLRT_DIV))
    MyIIC_WriteByte(GY520_ADDR, GY520_SMPLRT_DIV, 0x07);
    
    // 设置配置寄存器 - 低通滤波
    MyIIC_WriteByte(GY520_ADDR, GY520_CONFIG, 0x06);
    
    // 设置陀螺仪量程为±250°/s
    GY520_SetGyroRange(GY520_GYRO_FS_250);
    
    // 设置加速度计量程为±2g
    GY520_SetAccelRange(GY520_ACCEL_FS_2G);
    
    // 设置电源管理 - 使用内部8MHz振荡器
    MyIIC_WriteByte(GY520_ADDR, GY520_PWR_MGMT_1, 0x01);
    
    return 0;
}

u8 GY520_GetID(void)
{
    return MyIIC_ReadByte(GY520_ADDR, GY520_WHO_AM_I);
}

u8 GY520_GetRawData(GY520_Data_t *data)
{
    u8 buf[14];
    
    // 读取14个寄存器数据（加速度计、温度、陀螺仪）
    if (MyIIC_ReadBuffer(GY520_ADDR, GY520_ACCEL_XOUT_H, buf, 14)) return 1;
    
    // 转换为16位数据
    data->accelX = (buf[0] << 8) | buf[1];
    data->accelY = (buf[2] << 8) | buf[3];
    data->accelZ = (buf[4] << 8) | buf[5];
    data->temp = (buf[6] << 8) | buf[7];
    data->gyroX = (buf[8] << 8) | buf[9];
    data->gyroY = (buf[10] << 8) | buf[11];
    data->gyroZ = (buf[12] << 8) | buf[13];
    
    return 0;
}

u8 GY520_GetScaledData(GY520_Data_t *data)
{
    // 先获取原始数据
    if (GY520_GetRawData(data)) return 1;
    
    // 转换为实际物理单位
    data->accelX_g = data->accelX / accel_scale;
    data->accelY_g = data->accelY / accel_scale;
    data->accelZ_g = data->accelZ / accel_scale;
    
    data->gyroX_dps = data->gyroX / gyro_scale;
    data->gyroY_dps = data->gyroY / gyro_scale;
    data->gyroZ_dps = data->gyroZ / gyro_scale;
    
    // 温度转换公式：TEMP_OUT = 340 * TEMP_C + 3653
    data->temp_C = (data->temp - 3653) / 340.0f;
    
    return 0;
}

u8 GY520_SetGyroRange(u8 range)
{
    // 设置陀螺仪量程
    MyIIC_WriteByte(GY520_ADDR, GY520_GYRO_CONFIG, range);
    
    // 更新陀螺仪缩放系数
    switch (range)
    {
        case GY520_GYRO_FS_250:
            gyro_scale = 131.0f;
            break;
        case GY520_GYRO_FS_500:
            gyro_scale = 65.5f;
            break;
        case GY520_GYRO_FS_1000:
            gyro_scale = 32.8f;
            break;
        case GY520_GYRO_FS_2000:
            gyro_scale = 16.4f;
            break;
        default:
            return 1;
    }
    
    return 0;
}

u8 GY520_SetAccelRange(u8 range)
{
    // 设置加速度计量程
    MyIIC_WriteByte(GY520_ADDR, GY520_ACCEL_CONFIG, range);
    
    // 更新加速度计缩放系数
    switch (range)
    {
        case GY520_ACCEL_FS_2G:
            accel_scale = 16384.0f;
            break;
        case GY520_ACCEL_FS_4G:
            accel_scale = 8192.0f;
            break;
        case GY520_ACCEL_FS_8G:
            accel_scale = 4096.0f;
            break;
        case GY520_ACCEL_FS_16G:
            accel_scale = 2048.0f;
            break;
        default:
            return 1;
    }
    
    return 0;
}
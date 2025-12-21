#ifndef __GY520_H__
#define __GY520_H__

#include "myiic.h"
#include "stdint.h"

// GY-520 (MPU-6050) I2C地址
#define GY520_ADDR             0x68  // AD0接地时为0x68，接VCC时为0x69

// MPU-6050寄存器定义
#define GY520_SMPLRT_DIV       0x19  // 采样率分频器
#define GY520_CONFIG           0x1A  // 配置寄存器
#define GY520_GYRO_CONFIG      0x1B  // 陀螺仪配置
#define GY520_ACCEL_CONFIG     0x1C  // 加速度计配置
#define GY520_FIFO_EN          0x23  // FIFO使能
#define GY520_INT_PIN_CFG      0x37  // 中断引脚配置
#define GY520_INT_ENABLE       0x38  // 中断使能
#define GY520_INT_STATUS       0x3A  // 中断状态
#define GY520_ACCEL_XOUT_H     0x3B  // 加速度计X轴高字节
#define GY520_ACCEL_XOUT_L     0x3C  // 加速度计X轴低字节
#define GY520_ACCEL_YOUT_H     0x3D  // 加速度计Y轴高字节
#define GY520_ACCEL_YOUT_L     0x3E  // 加速度计Y轴低字节
#define GY520_ACCEL_ZOUT_H     0x3F  // 加速度计Z轴高字节
#define GY520_ACCEL_ZOUT_L     0x40  // 加速度计Z轴低字节
#define GY520_TEMP_OUT_H       0x41  // 温度传感器高字节
#define GY520_TEMP_OUT_L       0x42  // 温度传感器低字节
#define GY520_GYRO_XOUT_H      0x43  // 陀螺仪X轴高字节
#define GY520_GYRO_XOUT_L      0x44  // 陀螺仪X轴低字节
#define GY520_GYRO_YOUT_H      0x45  // 陀螺仪Y轴高字节
#define GY520_GYRO_YOUT_L      0x46  // 陀螺仪Y轴低字节
#define GY520_GYRO_ZOUT_H      0x47  // 陀螺仪Z轴高字节
#define GY520_GYRO_ZOUT_L      0x48  // 陀螺仪Z轴低字节
#define GY520_PWR_MGMT_1       0x6B  // 电源管理1
#define GY520_PWR_MGMT_2       0x6C  // 电源管理2
#define GY520_WHO_AM_I         0x75  // 设备ID

// 陀螺仪满量程选择
#define GY520_GYRO_FS_250      0x00  // ±250°/s
#define GY520_GYRO_FS_500      0x08  // ±500°/s
#define GY520_GYRO_FS_1000     0x10  // ±1000°/s
#define GY520_GYRO_FS_2000     0x18  // ±2000°/s

// 加速度计满量程选择
#define GY520_ACCEL_FS_2G      0x00  // ±2g
#define GY520_ACCEL_FS_4G      0x08  // ±4g
#define GY520_ACCEL_FS_8G      0x10  // ±8g
#define GY520_ACCEL_FS_16G     0x18  // ±16g

// 数据结构体
typedef struct
{
    int16_t accelX;  // 加速度计X轴原始值
    int16_t accelY;  // 加速度计Y轴原始值
    int16_t accelZ;  // 加速度计Z轴原始值
    int16_t gyroX;   // 陀螺仪X轴原始值
    int16_t gyroY;   // 陀螺仪Y轴原始值
    int16_t gyroZ;   // 陀螺仪Z轴原始值
    int16_t temp;    // 温度原始值
    
    float accelX_g;  // 加速度计X轴（g）
    float accelY_g;  // 加速度计Y轴（g）
    float accelZ_g;  // 加速度计Z轴（g）
    float gyroX_dps; // 陀螺仪X轴（°/s）
    float gyroY_dps; // 陀螺仪Y轴（°/s）
    float gyroZ_dps; // 陀螺仪Z轴（°/s）
    float temp_C;    // 温度（℃）
} GY520_Data_t;

// 函数声明
u8 GY520_Init(void);
u8 GY520_GetID(void);
u8 GY520_GetRawData(GY520_Data_t *data);
u8 GY520_GetScaledData(GY520_Data_t *data);
u8 GY520_SetGyroRange(u8 range);
u8 GY520_SetAccelRange(u8 range);

#endif // __GY520_H__
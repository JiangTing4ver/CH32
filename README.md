# CH32F03C8T6 项目说明

## 项目概述

这是一个基于 CH32F03C8T6 微控制器的开发项目，使用了 WCH（南京沁恒）提供的标准外设库。当前实现了一个简单的 LED 点亮功能，可作为基础项目框架进行扩展。

## 硬件信息

- **微控制器**: CH32F03C8T6 (Cortex-M3 内核)
- **主频**: 8MHz (可通过配置提升)
- **Flash**: 64KB
- **RAM**: 20KB
- **引脚数量**: 48

## 软件环境

- **开发工具**: Embedded IDE (EIDE) 或 Keil MDK
- **编译器**: ARM Compiler 6 (AC6)
- **固件库**: WCH32F1xx_DFP.1.0.2

## 项目结构

```
├── Application/        # 应用程序代码
│   └── main.c          # 主函数入口
├── CMSIS/              # CMSIS 标准库
│   ├── core_cm3.h      # Cortex-M3 核心头文件
│   ├── startup_ch32f10x.s  # 启动文件
│   ├── system_ch32f10x.c/h  # 系统时钟配置
│   └── delay.c/h       # 延时函数
├── Peripheral/         # 外设驱动库
│   ├── inc/            # 外设头文件
│   └── src/            # 外设源文件
├── Utilities/          # 实用工具
│   ├── ch32f10x_conf.h # 外设配置文件
│   └── ch32f10x_it.c/h # 中断处理函数
└── .eide/              # EIDE 项目配置
```

## 编译方法

### 使用 EIDE
1. 打开 VS Code 并安装 EIDE 插件
2. 打开项目文件夹
3. 在 EIDE 面板中选择 "Debug" 目标
4. 点击 "构建" 按钮

### 使用命令行
```bash
# 重新构建项目
unify_builder -p build/Debug/builder.params --rebuild
```

## 烧录方法

1. 使用 ST-Link 或 J-Link 连接开发板
2. 在 EIDE 面板中点击 "烧录" 按钮
3. 或使用 Keil MDK 进行烧录

## 示例功能

当前 `main.c` 中实现了一个简单的 LED 点亮功能：

- 初始化系统时钟
- 配置 PC13 引脚为推挽输出
- 点亮 LED (PC13 低电平有效)

## 扩展建议

1. **添加串口通信**: 使用 USART 实现与 PC 的通信
2. **添加传感器支持**: 如温湿度传感器、光照传感器等
3. **添加显示功能**: 使用 LCD 或 OLED 显示信息
4. **实现 USB 功能**: 利用 CH32F03C8T6 的 USB 接口实现设备功能
5. **添加无线通信**: 如蓝牙、WiFi 模块等

## 注意事项

1. 确保使用正确的固件库版本
2. 编译前检查配置文件中的时钟设置
3. 烧录时注意选择正确的设备型号
4. 扩展功能时注意引脚冲突
5. 进行 USB 开发时，需要正确配置 USB 描述符

## 许可证

本项目基于 WCH 提供的标准外设库开发，遵循相关开源协议。

## 联系方式

如有问题或建议，请随时提出 Issues 或 Pull Requests。
# CH32F03C8T6 项目说明

## 项目概述

这是一个基于 CH32F03C8T6 微控制器的多功能开发项目，使用了 WCH（南京沁恒）提供的标准外设库。项目采用模块化设计，已实现多个实用功能模块，包括基础 LED 控制、传感器数据采集、GY520 陀螺仪应用、串口通信以及 WS2812 灯带控制。项目结构清晰，易于扩展，适合嵌入式开发学习和二次开发。

### 项目特点
- 模块化设计，各功能模块独立，便于切换和扩展
- 详细的注释和文档，便于理解和使用
- 支持多种灯光效果（WS2812灯带）
- 提供多种传感器接口
- 支持串口通信，便于数据传输和调试

## 硬件信息

- **微控制器**: CH32F03C8T6 (Cortex-M3 内核)
- **主频**: 8MHz (可通过配置提升)
- **Flash**: 64KB
- **RAM**: 20KB
- **引脚数量**: 48

## 项目结构

```
├── Application/        # 应用程序代码
│   └── main.c          # 主函数入口
├── CMSIS/              # CMSIS 标准库
│   ├── ch32f10x.h      # 设备头文件
│   ├── cmsis_armclang.h # CMSIS 编译器支持
│   ├── cmsis_compiler.h # CMSIS 编译器宏
│   ├── core_cm3.h      # Cortex-M3 核心头文件
│   ├── debug.h         # 调试相关定义
│   ├── delay.c/h       # 延时函数
│   ├── startup_ch32f10x.s  # 启动文件
│   ├── system_ch32f10x.c/h  # 系统时钟配置
├── MyLib/              # 自定义功能库
│   ├── GPIO_contrl.c/h  # GPIO 控制
│   ├── HOME.h          # 库头文件汇总
│   ├── LED.c/h         # LED 控制
│   ├── MH_Sersor.c/h   # 传感器驱动
│   ├── MyUart.c/h      # 串口通信
│   ├── WS2812.c/h      # WS2812 灯带控制
│   ├── gy520.c/h       # GY520 陀螺仪驱动
│   └── myiic.c/h       # I2C 通信库
├── Peripheral/         # 外设驱动库
│   ├── inc/            # 外设头文件
│   └── src/            # 外设源文件
├── Utilities/          # 实用工具
│   ├── ch32f10x_conf.h # 外设配置文件
│   └── ch32f10x_it.c/h # 中断处理函数
├── .eide/              # EIDE 项目配置
├── .pack/              # Keil 固件包
├── .vscode/            # VS Code 配置
└── CH32.code-workspace # VS Code 工作区文件
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

## 功能模块

### LED 控制模块
- **功能**: 实现 LED 灯的初始化和闪烁功能
- **文件**: `MyLib/LED.c` 和 `MyLib/LED.h`
- **使用**: 在 `main.c` 中定义 `#define flag LED` 启用

### 传感器模块
- **功能**: 实现 MH 系列传感器的数据采集和处理
- **文件**: `MyLib/MH_Sersor.c` 和 `MyLib/MH_Sersor.h`
- **使用**: 在 `main.c` 中定义 `#define flag HM` 启用

### GY520 陀螺仪模块
- **功能**: 实现 GY520 陀螺仪的数据采集和处理
- **文件**: `MyLib/gy520.c` 和 `MyLib/gy520.h`
- **使用**: 在 `main.c` 中定义 `#define flag GY520` 启用

### 串口通信模块
- **功能**: 实现 USART1 的配置和通信功能
- **文件**: `MyLib/MyUart.c` 和 `MyLib/MyUart.h`
- **使用**: 在 `main.c` 中定义 `#define flag MyUSART1` 启用

### WS2812 灯带控制模块
- **功能**: 实现 WS2812 灯带的初始化和各种灯光效果
- **文件**: `MyLib/WS2812.c` 和 `MyLib/WS2812.h`
- **使用**: 在 `main.c` 中定义 `#define flag WS2812` 启用
- **配置**: 灯带默认配置为8个LED灯珠（`WS2812_NUM_LEDS 8`）
- **效果**: 支持呼吸灯、流水灯、彩虹流水灯等多种效果

### 蜂鸣器
- **功能**: 实现蜂鸣器的初始化和控制
- **文件**: `MyLib/Buzzer.c` 和 `MyLib/Buzzer.h`
- **使用**: 在 `main.c` 中定义 `#define flag Buzzer` 启用

如何区分有源蜂鸣器和无源蜂鸣器？
- 有源蜂鸣器：内部包含振荡电路，直接供电即可工作
- 无源蜂鸣器：需要外部提供振荡信号才能工作
  [有源蜂鸣器和无源区分：手把手教你辨认方法](https://blog.csdn.net/weixin_35753291/article/details/156169056?ops_request_misc=%257B%2522request%255Fid%2522%253A%2522a992eb79840ff70eae87e331817e9a7d%2522%252C%2522scm%2522%253A%252220140713.130102334.pc%255Fall.%2522%257D&request_id=a992eb79840ff70eae87e331817e9a7d&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~all~first_rank_ecpm_v1~rank_v31_ecpm-9-156169056-null-null.142^v102^pc_search_result_base7&utm_term=%E6%9C%89%E6%BA%90%E8%9C%82%E9%B8%A3%E5%99%A8%E5%92%8C%E6%97%A0%E6%BA%90%E5%8C%BA%E5%88%86)


### 编译与烧录

1. 编译程序：
   - 使用 EIDE：在 EIDE 面板中选择 "Debug" 目标，点击 "构建" 按钮
   - 使用命令行：
     ```bash
     # 重新构建项目
     unify_builder -p build/Debug/builder.params --rebuild
     ```

2. 烧录程序：
   - 使用 ST-Link 或 J-Link 连接开发板
   - 在 EIDE 面板中点击 "烧录" 按钮
   - 或使用 Keil MDK 进行烧录

3. 观察效果：
   - 烧录完成后，开发板上的对应功能模块将开始工作
   - 对于 WS2812 模块，您将看到灯带按照编程的效果变化

## 扩展建议

1. **添加更多传感器支持**: 如温湿度传感器、光照传感器等
2. **添加显示功能**: 使用 LCD 或 OLED 显示信息
3. **实现 USB 功能**: 利用 CH32F03C8T6 的 USB 接口实现设备功能
4. **添加无线通信**: 如蓝牙、WiFi 模块等
5. **优化现有功能**: 提升灯光效果的多样性、优化传感器数据处理算法等

## 注意事项

1. 确保使用正确的固件库版本
2. 编译前检查配置文件中的时钟设置
3. 烧录时注意选择正确的设备型号
4. 扩展功能时注意引脚冲突
5. 切换功能模块时，确保只定义一个 `flag` 宏
6. 使用 WS2812 功能时，确保正确连接灯带引脚

## 许可证

本项目基于 WCH 提供的标准外设库开发，遵循相关开源协议。

## 联系方式

如有问题或建议，请随时提出 Issues 或 Pull Requests。
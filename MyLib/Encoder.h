#ifndef ENCODER_H
#define ENCODER_H
// ---------------头文件------------------
#include "ch32f10x.h"
// ---------------宏定义------------------

// ---------------函数声明------------------
void Encoder_Init(void);
int16_t Encoder_Get(void);

#endif
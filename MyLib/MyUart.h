#ifndef __MYUART_H__
#define __MYUART_H__

#include "ch32f10x_gpio.h"
#include "ch32f10x_rcc.h"
#include "ch32f10x_usart.h"
#include "ch32f10x_dma.h"
#include "ch32f10x_misc.h"
#include <stdio.h>
#include <string.h>
#include "stddef.h"
#include "LED.h"

/************************************************************
						变量声明
*************************************************************/


/************************************************************
						函数声明
*************************************************************/
void USART1_Config(uint32_t BaudRate);
void Usart_SendByte(USART_TypeDef* pUSARTx,uint8_t date);
void Usart_SendHalfWord(USART_TypeDef* pUSARTx,uint16_t date);
void Usart_SendString( USART_TypeDef * pUSARTx, char *str);
int fputc(int ch, FILE *f);
int fgetc(FILE *f);
void RX_Process(void);

#endif // __MYUART_H__

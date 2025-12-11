/********************************** (C) COPYRIGHT  *******************************
 * File Name          : debug.h
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2019/10/15
 * Description        : Debug output header file.
 *********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#ifndef __DEBUG_H
#define __DEBUG_H

#ifdef __cplusplus
extern "C" {
#endif

// Define __packed and __align for ARM Clang compiler
#ifndef __packed
#define __packed __attribute__((packed))
#endif

#ifndef __align
#define __align(x) __attribute__((aligned(x)))
#endif

/* Debug output functions */
#define DEBUG_PRINTF(...)    /* No operation by default */
#define DEBUG_INFO(...)      /* No operation by default */
#define DEBUG_ERROR(...)     /* No operation by default */

#ifdef __cplusplus
}
#endif

#endif /* __DEBUG_H */
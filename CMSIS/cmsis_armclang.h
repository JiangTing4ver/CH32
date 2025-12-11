/**************************************************************************//**
 * @file     cmsis_armclang.h
 * @brief    CMSIS compiler armclang (ARM Compiler 6) header file
 * @version  V5.0.5
 * @date     20. January 2017
 ******************************************************************************/
/*
 * Copyright (c) 2009-2017 ARM Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __CMSIS_ARMCLANG_H
#define __CMSIS_ARMCLANG_H

/* CMSIS compiler specific defines */

/* define compiler specific symbols */
#ifndef   __ASM
  #define __ASM            __asm
#endif
#ifndef   __INLINE
  #define __INLINE         inline
#endif
#ifndef   __STATIC_INLINE
  #define __STATIC_INLINE  static inline
#endif
#ifndef   __NO_RETURN
  #define __NO_RETURN      __attribute__((noreturn))
#endif
#ifndef   __USED
  #define __USED           __attribute__((used))
#endif
#ifndef   __WEAK
  #define __WEAK           __attribute__((weak))
#endif
#ifndef   __PACKED
  #define __PACKED         __attribute__((packed))
#endif
#ifndef   __PACKED_STRUCT
  #define __PACKED_STRUCT  struct __attribute__((packed))
#endif
#ifndef   __UNALIGNED_UINT32
  #define __UNALIGNED_UINT32(x)  (*(volatile uint32_t *)((uint8_t *)(x)))
#endif
#ifndef   __UNALIGNED_UINT16
  #define __UNALIGNED_UINT16(x)  (*(volatile uint16_t *)((uint8_t *)(x)))
#endif
#ifndef   __UNALIGNED_UINT8
  #define __UNALIGNED_UINT8(x)   (*(volatile uint8_t *)(x))
#endif
#ifndef   __ALIGNED
  #define __ALIGNED(x)     __attribute__((aligned(x)))
#endif

/* define compiler memory barrier */
#ifndef   __DSB
  #define __DSB()          __asm volatile ("dsb 0xF":::"memory")
#endif
#ifndef   __DMB
  #define __DMB()          __asm volatile ("dmb 0xF":::"memory")
#endif
#ifndef   __ISB
  #define __ISB()          __asm volatile ("isb 0xF":::"memory")
#endif

/* define compiler specific I/O register access functions */
#ifndef   __IO
  #define __IO volatile
#endif

/* IO definitions (access restrictions to peripheral registers) */
#ifdef __cplusplus
  #define     __I     volatile             /*!< Defines 'read only' permissions */
#else
  #define     __I     volatile const       /*!< Defines 'read only' permissions */
#endif
#define     __O     volatile               /*!< Defines 'write only' permissions */
#define     __IO    volatile               /*!< Defines 'read / write' permissions */

/*@} end of group IO_definitions */

/**
 * @ingroup  CMSIS_Core_FunctionInterface
 * @defgroup CMSIS_Core_RegAccFunctions  Register Access Functions
 * @{
 */

/* compiler specific macros for register access */

/**
 * @brief  Read a bitfield from a register
 * @param  __REG      Register to read
 * @param  __BFLD     Bitfield to read (defined as shift and mask)
 * @return            Bitfield value read from the register
 */
#define __GET_BITFIELD(__REG, __BFLD)      (((__REG) & (__BFLD ## _Msk)) >> (__BFLD ## _Pos))

/**
 * @brief  Write a bitfield to a register
 * @param  __REG      Register to write
 * @param  __BFLD     Bitfield to write (defined as shift and mask)
 * @param  __VAL      Value to write to bitfield
 */
#define __SET_BITFIELD(__REG, __BFLD, __VAL) \
  ((__REG) = ((__REG) & (~(__BFLD ## _Msk))) | (((__VAL) << (__BFLD ## _Pos)) & (__BFLD ## _Msk)))

/**
 * @brief  Set bits in a register
 * @param  __REG      Register to write
 * @param  __VAL      Value to set in register
 */
#define __SET_REG(__REG, __VAL)            ((__REG) = (__VAL))

/**
 * @brief  Get register value
 * @param  __REG      Register to read
 * @return            Register value
 */
#define __GET_REG(__REG)                   ((__REG))

/**
 * @brief  Set a bit in a register
 * @param  __REG      Register to write
 * @param  __BIT      Bit position to set
 */
#define __SET_BIT(__REG, __BIT)            ((__REG) |= (__BIT))

/**
 * @brief  Clear a bit in a register
 * @param  __REG      Register to write
 * @param  __BIT      Bit position to clear
 */
#define __CLEAR_BIT(__REG, __BIT)          ((__REG) &= ~(__BIT))

/**
 * @brief  Toggle a bit in a register
 * @param  __REG      Register to write
 * @param  __BIT      Bit position to toggle
 */
#define __TOGGLE_BIT(__REG, __BIT)         ((__REG) ^= (__BIT))

/**
 * @brief  Check if a bit is set in a register
 * @param  __REG      Register to read
 * @param  __BIT      Bit position to check
 * @return            Non-zero if bit is set, zero otherwise
 */
#define __READ_BIT(__REG, __BIT)           ((__REG) & (__BIT))

/**
 * @brief  Wait for a bit to be set in a register
 * @param  __REG      Register to read
 * @param  __BIT      Bit position to wait for
 */
#define __WFI()                            __asm volatile ("wfi")

/**
 * @brief  Wait for event
 * @param  __REG      Register to read
 * @param  __BIT      Bit position to wait for
 */
#define __WFE()                            __asm volatile ("wfe")

/**
 * @brief  Send event
 */
#define __SEV()                            __asm volatile ("sev")

/**
 * @brief  Instruction Synchronization Barrier
 */
#define __ISB()                            __asm volatile ("isb 0xF":::"memory")

/**
 * @brief  Data Synchronization Barrier
 */
#define __DSB()                            __asm volatile ("dsb 0xF":::"memory")

/**
 * @brief  Data Memory Barrier
 */
#define __DMB()                            __asm volatile ("dmb 0xF":::"memory")

/**
 * @brief  Return from interrupt
 */
#define __REV()                            __builtin_bswap32
#define __REV16()                          __builtin_bswap16
#define __REVSH()                          ((int16_t)__builtin_bswap16((uint16_t)__val))
#define __RBIT()                           __builtin_bitreverse32
#define __NOP()                            __asm volatile ("nop")

/**
 * @brief  Get System Counter value
 * @return System Counter value
 */
#define __GET_SP()                         __asm volatile ("mov %0, sp" : "=r" (__sp))
#define __SET_SP(__sp)                     __asm volatile ("mov sp, %0" : : "r" (__sp))

/**
 * @brief  Enable IRQ interrupts
 */
#define __enable_irq()                     __asm volatile ("cpsie i" ::: "memory")

/**
 * @brief  Disable IRQ interrupts
 */
#define __disable_irq()                    __asm volatile ("cpsid i" ::: "memory")

/**
 * @brief  Get Priority Mask
 * @return Priority Mask value
 */
#define __get_PRIMASK()                    ({ uint32_t __primask; __asm volatile ("mrs %0, primask" : "=r" (__primask)); __primask; })

/**
 * @brief  Set Priority Mask
 * @param  __primask  Priority Mask value to set
 */
#define __set_PRIMASK(__primask)           __asm volatile ("msr primask, %0" : : "r" (__primask) : "memory")

/**
 * @brief  Get Base Priority
 * @return Base Priority value
 */
#define __get_BASEPRI()                    ({ uint32_t __basepri; __asm volatile ("mrs %0, basepri" : "=r" (__basepri)); __basepri; })

/**
 * @brief  Set Base Priority
 * @param  __basepri  Base Priority value to set
 */
#define __set_BASEPRI(__basepri)           __asm volatile ("msr basepri, %0" : : "r" (__basepri) : "memory")

/**
 * @brief  Get Fault Mask
 * @return Fault Mask value
 */
#define __get_FAULTMASK()                  ({ uint32_t __faultmask; __asm volatile ("mrs %0, faultmask" : "=r" (__faultmask)); __faultmask; })

/**
 * @brief  Set Fault Mask
 * @param  __faultmask  Fault Mask value to set
 */
#define __set_FAULTMASK(__faultmask)       __asm volatile ("msr faultmask, %0" : : "r" (__faultmask) : "memory")

/**
 * @brief  Get Control Register value
 * @return Control Register value
 */
#define __get_CONTROL()                    ({ uint32_t __control; __asm volatile ("mrs %0, control" : "=r" (__control)); __control; })

/**
 * @brief  Set Control Register value
 * @param  __control  Control Register value to set
 */
#define __set_CONTROL(__control)           __asm volatile ("msr control, %0" : : "r" (__control) : "memory")

/*@} end of group CMSIS_Core_RegAccFunctions */

#endif /* __CMSIS_ARMCLANG_H */
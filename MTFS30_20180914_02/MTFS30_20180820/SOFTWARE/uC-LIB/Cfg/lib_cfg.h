/*
*********************************************************************************************************
*                                               uC/LIB
*                                       CUSTOM LIBRARY MODULES
*
*                          (c) Copyright 2004-2009; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*
*               uC/LIB is provided in source form for FREE evaluation, for educational
*               use or peaceful research.  If you plan on using uC/LIB in a commercial
*               product you need to contact Micrium to properly license its use in your
*               product.  We provide ALL the source code for your convenience and to
*               help you experience uC/LIB.  The fact that the source code is provided
*               does NOT mean that you can use it without paying a licensing fee.
*
*               Knowledge of the source code may NOT be used to develop a similar product.
*
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                  CUSTOM LIBRARY CONFIGURATION FILE
*
*                                              TEMPLATE
*
* Filename      : lib_cfg.h
* Version       : V1.30
* Programmer(s) : ITJ
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                               MODULE
*********************************************************************************************************
*/

#ifndef  LIB_CFG_MODULE_PRESENT
#define  LIB_CFG_MODULE_PRESENT


/*$PAGE*/
/*
*********************************************************************************************************
*********************************************************************************************************
*                                    MEMORY LIBRARY CONFIGURATION
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                             MEMORY LIBRARY ARGUMENT CHECK CONFIGURATION
*
* Note(s) : (1) Configure LIB_MEM_CFG_ARG_CHK_EXT_EN to enable/disable the memory library suite external
*               argument check feature :
*
*               (a) When ENABLED,      arguments received from any port interface provided by the developer
*                   or application are checked/validated.
*
*               (b) When DISABLED, NO  arguments received from any port interface provided by the developer
*                   or application are checked/validated.
*********************************************************************************************************
*/

                                                        /* Configure external argument check feature (see Note #1) :    */
#define  LIB_MEM_CFG_ARG_CHK_EXT_EN     DEF_DISABLED
                                                        /*   DEF_DISABLED     Argument check DISABLED                   */
                                                        /*   DEF_ENABLED      Argument check ENABLED                    */


/*
*********************************************************************************************************
*                         MEMORY LIBRARY ASSEMBLY OPTIMIZATION CONFIGURATION
*
* Note(s) : (1) Configure LIB_MEM_CFG_OPTIMIZE_ASM_EN to enable/disable assembly-optimized memory function(s).
*********************************************************************************************************
*/

                                                        /* Configure assembly-optimized function(s) [see Note #1] :     */
#define  LIB_MEM_CFG_OPTIMIZE_ASM_EN    DEF_DISABLED
                                                        /*   DEF_DISABLED     Assembly-optimized function(s) DISABLED   */
                                                        /*   DEF_ENABLED      Assembly-optimized function(s) ENABLED    */


/*
*********************************************************************************************************
*                                   MEMORY ALLOCATION CONFIGURATION
*
* Note(s) : (1) Configure LIB_MEM_CFG_ALLOC_EN to enable/disable memory allocation functions.
*
*           (2) (a) Configure LIB_MEM_CFG_HEAP_SIZE with the desired size of heap memory (in octets).
*
*               (b) Configure LIB_MEM_CFG_HEAP_BASE_ADDR to specify a base address for heap memory :
*
*                   (a) Heap initialized to specified application memory,  if LIB_MEM_CFG_HEAP_BASE_ADDR
*                                                                                 #define'd in 'app_cfg.h'
*
*                   (b) Heap declared in 'lib_mem.c',                      if LIB_MEM_CFG_HEAP_BASE_ADDR
*                                                                             NOT #define'd in 'app_cfg.h'
*********************************************************************************************************
*/

                                                        /* Configure memory allocation feature (see Note #1) :          */
#define  LIB_MEM_CFG_ALLOC_EN           DEF_DISABLED
                                                        /*   DEF_DISABLED     Memory allocation DISABLED                */
                                                        /*   DEF_ENABLED      Memory allocation ENABLED                 */


#define  LIB_MEM_CFG_HEAP_SIZE                  1024    /* Configure heap memory size         [see Note #2a].           */

#if 0                                                   /* Configure heap memory base address (see Note #2b).           */
#define  LIB_MEM_CFG_HEAP_BASE_ADDR
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*********************************************************************************************************
*                                    STRING LIBRARY CONFIGURATION
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                 STRING FLOATING POINT CONFIGURATION
*
* Note(s) : (1) Configure LIB_STR_CFG_FP_EN to enable/disable floating point string function(s).
*********************************************************************************************************
*/

                                                        /* Configure floating point feature(s) [see Note #1] :          */
#define  LIB_STR_CFG_FP_EN              DEF_DISABLED
                                                        /*   DEF_DISABLED     Floating point functions DISABLED         */
                                                        /*   DEF_ENABLED      Floating point functions ENABLED          */


/*$PAGE*/
/*
*********************************************************************************************************
*                                             MODULE END
*********************************************************************************************************
*/

#endif                                                  /* End of lib cfg module include.                               */


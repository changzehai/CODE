/*****************************************************************************/
/* �ļ���:    util_check.h                                                   */
/* ��  ��:    У�鴦����ͷ�ļ�                                               */
/* ��  ��:    2018-07-19 changzehai(DTT)                                     */
/* ��  ��:    ��                                                             */
/* Copyright 1998 - 2018 DTT. All Rights Reserved                            */
/*****************************************************************************/
#ifndef __UTIL_CHECK_H_
#define __UTIL_CHECK_H_
#include "cc.h"



/*-------------------------------*/
/* �궨��                        */
/*-------------------------------*/
#define  CHECKCODE_EQ      0     /* У������ͬ */
#define  CHECKCODE_NOEQ    1     /* У���벻ͬ */

/* У�鷽ʽ */
#define CHECK_TYPE_XOR     1     /* ���У��   */
#define CHECK_TYPE_CRC32   2     /* crc32У��  */


/*-------------------------------*/
/* ��������                      */
/*-------------------------------*/
extern u8_t util_check_crc32(const u8_t *str, u32_t len, const u8_t* chekcode);
extern u8_t util_xor_check(const u8_t *str, u32_t len, const u8_t* chekcode);
#endif

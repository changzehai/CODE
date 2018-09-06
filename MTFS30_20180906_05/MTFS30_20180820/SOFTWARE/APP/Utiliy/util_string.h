/*****************************************************************************/
/* �ļ���:    util_string.h                                                  */
/* ��  ��:    �ַ�����ش���ͷ�ļ�                                           */
/* ��  ��:    2018-07-20 changzehai(DTT)                                     */
/* ��  ��:    ��                                                             */
/* Copyright 1998 - 2018 DTT. All Rights Reserved                            */
/*****************************************************************************/
#ifndef __UTIL_STRING_H_
#define __UTIL_STRING_H_
#include "cc.h"


/*-------------------------------*/
/* �궨��                        */
/*-------------------------------*/
#define SUB_STRING_MAX_LEN   128    /* �ִ���󳤶�,��'\0' */

/*-------------------------------*/
/* ��������                      */
/*-------------------------------*/
extern  u32_t str_to_int32u(u8_t *str);
extern  s32_t str_to_int32s(u8_t *str);
extern u8_t  my_strtok(char  ch, const char *str, char *sub_str, u16_t sub_str_size);
extern  u32_t str16_to_int32u(const u8_t *str);
#endif

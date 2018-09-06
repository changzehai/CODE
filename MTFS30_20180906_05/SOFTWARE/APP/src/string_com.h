/* 文件名:    string_com.h                                                   */
/* 描  述:    字符串相关处理头文件                                           */
/* 创  建:    2018-07-20 changzehai(DTT)                                     */
/* 更  新:    无                                                             */
/* Copyright 1998 - 2018 DTT. All Rights Reserved                            */
/*****************************************************************************/
#include "mtfs30_com.h"


/*-------------------------------*/
/* 宏定义                        */
/*-------------------------------*/
#define SUB_STRING_MAX_LEN   128    /* 字串最大长度,带'\0' */

/*-------------------------------*/
/* 函数声明                      */
/*-------------------------------*/
extern  u32_t str_to_int32u(u8_t *str);
extern  s32_t str_to_int32s(u8_t *str);
extern u8_t  my_strtok(char  ch, const char *str, char *sub_str, u16_t sub_str_size);

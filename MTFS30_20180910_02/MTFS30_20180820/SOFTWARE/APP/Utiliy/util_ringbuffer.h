/*****************************************************************************/
/* 文件名:    util_ringbuffer.h                                              */
/* 描  述:    环形缓冲区相关处理头文件                                       */
/* 创  建:    2018-07-20 changzehai(DTT)                                     */
/* 更  新:    无                                                             */
/* Copyright 1998 - 2018 DTT. All Rights Reserved                            */
/*****************************************************************************/
#ifndef __UTIL_RINGBUFFER_H_
#define __UTIL_RINGBUFFER_H_
#include "cc.h"


/* 环形缓冲区结构定义 */
typedef struct _RINGBUFFER_T_
{
    u8_t *prb_buf;    /* 指向缓冲区 */
    u16_t rb_write;   /* 写index    */
    u16_t rb_read;    /* 读index    */
    u16_t rb_size;    /* 缓冲区大小 */
} ringbuffer_t;


/* 宏函数 */
#define  min(a,b)  ( (a) < (b) ) ? (a):(b)   /* 求两个数的最小值 */

/*-------------------------------*/
/* 函数声明                      */
/*-------------------------------*/
extern void util_ringbuffer_create(ringbuffer_t *rb, u16_t rb_size);
extern u16_t util_ringbuffer_used(ringbuffer_t *rb);
extern u16_t util_ringbuffer_no_use(ringbuffer_t *rb);
extern u16_t util_ringbuffer_read(ringbuffer_t *rb, u8_t *data, u16_t read_size);
extern u16_t util_ringbuffer_write(ringbuffer_t *rb, u8_t *data, u16_t write_size);
//extern u8_t util_ringbuffer_get(ringbuffer_t *rb);
#endif

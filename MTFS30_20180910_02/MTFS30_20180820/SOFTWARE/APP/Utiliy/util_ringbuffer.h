/*****************************************************************************/
/* �ļ���:    util_ringbuffer.h                                              */
/* ��  ��:    ���λ�������ش���ͷ�ļ�                                       */
/* ��  ��:    2018-07-20 changzehai(DTT)                                     */
/* ��  ��:    ��                                                             */
/* Copyright 1998 - 2018 DTT. All Rights Reserved                            */
/*****************************************************************************/
#ifndef __UTIL_RINGBUFFER_H_
#define __UTIL_RINGBUFFER_H_
#include "cc.h"


/* ���λ������ṹ���� */
typedef struct _RINGBUFFER_T_
{
    u8_t *prb_buf;    /* ָ�򻺳��� */
    u16_t rb_write;   /* дindex    */
    u16_t rb_read;    /* ��index    */
    u16_t rb_size;    /* ��������С */
} ringbuffer_t;


/* �꺯�� */
#define  min(a,b)  ( (a) < (b) ) ? (a):(b)   /* ������������Сֵ */

/*-------------------------------*/
/* ��������                      */
/*-------------------------------*/
extern void util_ringbuffer_create(ringbuffer_t *rb, u16_t rb_size);
extern u16_t util_ringbuffer_used(ringbuffer_t *rb);
extern u16_t util_ringbuffer_no_use(ringbuffer_t *rb);
extern u16_t util_ringbuffer_read(ringbuffer_t *rb, u8_t *data, u16_t read_size);
extern u16_t util_ringbuffer_write(ringbuffer_t *rb, u8_t *data, u16_t write_size);
//extern u8_t util_ringbuffer_get(ringbuffer_t *rb);
#endif

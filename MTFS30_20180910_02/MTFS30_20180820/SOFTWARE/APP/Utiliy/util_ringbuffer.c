/*****************************************************************************/
/* �ļ���:    util_ringbuffer.c                                              */
/* ��  ��:    ���λ�������ش���                                             */
/* ��  ��:    2018-07-20 changzehai(DTT)                                     */
/* ��  ��:    ��                                                             */
/* Copyright 1998 - 2018 DTT. All Rights Reserved                            */
/*****************************************************************************/
#include "util_ringbuffer.h"
#include "string.h"
#include "cc.h"
#include "dev.h"
#include "mtfs30_debug.h"





/*****************************************************************************
 * ��  ��:    util_ringbuffer_create                                                          
 * ��  ��:    �������λ�����                                                                
 * ��  ��:    rb_size :  ���λ�������С              
 * ��  ��:    *rb : ָ�򴴽��Ļ��λ�����                                                    
 * ����ֵ:                                                        
 * ��  ��:    2018-07-18 changzehai(DTT)                            
 * ��  ��:    ��                                       
 ****************************************************************************/
void util_ringbuffer_create(ringbuffer_t *rb, u16_t rb_size)
{
 
    /* Ϊ���λ���������ռ� */
    rb->prb_buf = (u8_t *)malloc(rb_size);
    if (rb->prb_buf == NULL)
    {        
        return;
    }
   
    /* ��ʼ�� */
    rb->rb_write = 0;
    rb->rb_read = 0;
    rb->rb_size = rb_size;   
#if MTFS30_DEBUG_EN
            MTFS30_DEBUG("�������ζ��гɹ�������\n");     
#endif      
}

/*****************************************************************************
 * ��  ��:    util_ringbuffer_used                                                          
 * ��  ��:    ��ȡ���λ�������ʹ�õĿռ��С                                                                
 * ��  ��:    *rb : ָ�򴴽��Ļ��λ�����               
 * ��  ��:                                                       
 * ����ֵ:    ���λ�������ʹ�õĿռ��С                                                    
 * ��  ��:    2018-07-18 changzehai(DTT)                            
 * ��  ��:    ��                                       
 ****************************************************************************/
u16_t util_ringbuffer_used(ringbuffer_t *rb)
{

    return ((rb->rb_size + rb->rb_write - rb->rb_read) % rb->rb_size);
}


/*****************************************************************************
 * ��  ��:    util_ringbuffer_no_use                                                          
 * ��  ��:    ��ȡ���λ�����δʹ�õĿռ��С                                                                
 * ��  ��:    *rb : ָ�򴴽��Ļ��λ�����               
 * ��  ��:                                                       
 * ����ֵ:    ���λ�����δʹ�õĿռ��С                                                    
 * ��  ��:    2018-07-18 changzehai(DTT)                            
 * ��  ��:    ��                                       
 ****************************************************************************/
u16_t util_ringbuffer_no_use(ringbuffer_t *rb)
{
    /* ���λ������ܴ�С��ȥ��ʹ�õĿռ��С */
    return (rb->rb_size - util_ringbuffer_used(rb));
}

/*****************************************************************************
 * ��  ��:    util_ringbuffer_read                                                          
 * ��  ��:    �ӻ��λ������ж�ȡָ����С������                                                              
 * ��  ��:    *rb        : ָ�򴴽��Ļ��λ�����  
 *            *data      : ��Ŀ���ַ
 *            *read_size : ָ�����Ĵ�С
 * ��  ��:                                                       
 * ����ֵ:    ʵ�ʶ����Ĵ�С                                                   
 * ��  ��:    2018-07-18 changzehai(DTT)                            
 * ��  ��:    ��                                       
 ****************************************************************************/
u16_t util_ringbuffer_read(ringbuffer_t *rb, u8_t *data, u16_t read_size)
{
 
    u16_t used_size = 0;
    u16_t copy_size = 0;
    
    
    /* ȡ�ÿɶ����ݴ�С */
    used_size = util_ringbuffer_used(rb);
    
    /* �޿ɶ������ݣ�ֱ�ӷ��� */
    if (used_size == 0)
    {
        return 0;
    }
    
    /* ָ�����Ĵ�С�����ɶ��ռ�Ĵ�С����ֻ���ɶ��ռ��С������ /
    if (read_size > used_size)
    {
        read_size = used_size;
    }
    
    /* ���ָ�����Ĵ�СС�ڡ���index��������ĩβ���Ŀռ��С����ֱ�Ӷ�;
     *  ���򣬷����ζ����ȶ�����index��������ĩβ���Ŀռ��С�����ݣ�Ȼ��
     *  �ٴӻ�������ͷ���Ŷ�ʣ�������
     */ 
    copy_size = min(read_size, rb->rb_size - rb->rb_read);
    memcpy(data, rb->prb_buf+rb->rb_read, copy_size);
    memcpy(data+copy_size, rb->prb_buf, read_size-copy_size);
    
    
    /* ���¶�index */
    rb->rb_read =  (rb->rb_read + read_size) % rb->rb_size;
    

    return read_size; /* ����ʵ�ʶ��Ĵ�С */

}

/*****************************************************************************
 * ��  ��:    util_ringbuffer_get                                                          
 * ��  ��:    �ӻ��λ������ж�ȡһ���ֽڵ�����                                                              
 * ��  ��:    *rb        : ָ�򴴽��Ļ��λ�����  
 * ��  ��:                                                       
 * ����ֵ:    ������һ���ֽڵ�����                                                   
 * ��  ��:    2018-07-18 changzehai(DTT)                            
 * ��  ��:    ��                                       
 ****************************************************************************/
//u8_t util_ringbuffer_get(ringbuffer_t *rb)
//{
//    u8_t ch;
//    u16_t used_size = 0;
//    
//    /* ȡ�ÿɶ����ݴ�С */
//    used_size = util_ringbuffer_used(rb); 
//    
//    if (used_size == 0)
//    {
//        return 0;
//    }
//    
//    ch = rb->prb_buf[rb->rb_read];
//    rb->rb_read = (rb->rb_read + 1) % rb->rb_size;
//    
//    return ch;
//}


/*****************************************************************************
 * ��  ��:    util_ringbuffer_write                                                         
 * ��  ��:    ���λ�������д��ָ����С������                                                               
 * ��  ��:    *rb         : ָ�򴴽��Ļ��λ�����  
 *            *data       : д���ݵ�ַ
 *            *write_size : ָ��д�Ĵ�С
 * ��  ��:                                                       
 * ����ֵ:    ʵ��д��С                                                   
 * ��  ��:    2018-07-18 changzehai(DTT)                            
 * ��  ��:    ��                                       
 ****************************************************************************/
u16_t util_ringbuffer_write(ringbuffer_t *rb, u8_t *data, u16_t write_size)
{
   
    u16_t no_use_size = 0;
    u16_t copy_size = 0;
    
    
    /* ȡ�ÿ�д�ռ��С */
    no_use_size = util_ringbuffer_no_use(rb);
    
    /* û�пռ��д��ֱ�ӷ��� */
    if (no_use_size == 0)
    {
        return 0;
    }
    
    /* ������д�ռ��С����ֻд��д�ռ��С������ */
    if (write_size > no_use_size)
    {
        write_size = no_use_size;

    }
    
    /* ���ָ��д�Ĵ�СС�ڡ�дindex��������ĩβ���Ŀռ��С����ֱ��д;
     *  ���򣬷�����д����д��дindex��������ĩβ���Ŀռ��С�����ݣ�Ȼ��
     *  �ٴӻ�������ͷ����дʣ�������
     */      
    copy_size = min(write_size, rb->rb_size - rb->rb_write);    
    memcpy(rb->prb_buf + rb->rb_write, data, copy_size);
    memcpy(rb->prb_buf, data+copy_size, write_size-copy_size);
    
    /* ����дindex */
    rb->rb_write = (rb->rb_write + write_size) %  rb->rb_size;
    
    
    return write_size; /* ����ʵ��д�Ĵ�С */
}

/*****************************************************************************/
/* 文件名:    util_ringbuffer.c                                              */
/* 描  述:    环形缓冲区相关处理                                             */
/* 创  建:    2018-07-20 changzehai(DTT)                                     */
/* 更  新:    无                                                             */
/* Copyright 1998 - 2018 DTT. All Rights Reserved                            */
/*****************************************************************************/
#include "util_ringbuffer.h"
#include "string.h"
#include "cc.h"
#include "dev.h"
#include "mtfs30_debug.h"





/*****************************************************************************
 * 函  数:    util_ringbuffer_create                                                          
 * 功  能:    创建环形缓冲区                                                                
 * 输  入:    rb_size :  环形缓冲区大小              
 * 输  出:    *rb : 指向创建的环形缓冲区                                                    
 * 返回值:                                                        
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
void util_ringbuffer_create(ringbuffer_t *rb, u16_t rb_size)
{
 
    /* 为环形缓冲区分配空间 */
    rb->prb_buf = (u8_t *)malloc(rb_size);
    if (rb->prb_buf == NULL)
    {        
        return;
    }
   
    /* 初始化 */
    rb->rb_write = 0;
    rb->rb_read = 0;
    rb->rb_size = rb_size;   
#if MTFS30_DEBUG_EN
            MTFS30_DEBUG("创建环形队列成功！！！\n");     
#endif      
}

/*****************************************************************************
 * 函  数:    util_ringbuffer_used                                                          
 * 功  能:    获取环形缓冲区已使用的空间大小                                                                
 * 输  入:    *rb : 指向创建的环形缓冲区               
 * 输  出:                                                       
 * 返回值:    环形缓冲区已使用的空间大小                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
u16_t util_ringbuffer_used(ringbuffer_t *rb)
{

    return ((rb->rb_size + rb->rb_write - rb->rb_read) % rb->rb_size);
}


/*****************************************************************************
 * 函  数:    util_ringbuffer_no_use                                                          
 * 功  能:    获取环形缓冲区未使用的空间大小                                                                
 * 输  入:    *rb : 指向创建的环形缓冲区               
 * 输  出:                                                       
 * 返回值:    环形缓冲区未使用的空间大小                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
u16_t util_ringbuffer_no_use(ringbuffer_t *rb)
{
    /* 环形缓冲区总大小减去已使用的空间大小 */
    return (rb->rb_size - util_ringbuffer_used(rb));
}

/*****************************************************************************
 * 函  数:    util_ringbuffer_read                                                          
 * 功  能:    从环形缓冲区中读取指定大小的数据                                                              
 * 输  入:    *rb        : 指向创建的环形缓冲区  
 *            *data      : 读目标地址
 *            *read_size : 指定读的大小
 * 输  出:                                                       
 * 返回值:    实际读出的大小                                                   
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
u16_t util_ringbuffer_read(ringbuffer_t *rb, u8_t *data, u16_t read_size)
{
 
    u16_t used_size = 0;
    u16_t copy_size = 0;
    
    
    /* 取得可读数据大小 */
    used_size = util_ringbuffer_used(rb);
    
    /* 无可读的数据，直接返回 */
    if (used_size == 0)
    {
        return 0;
    }
    
    /* 指定读的大小超过可读空间的大小，则只读可读空间大小的数据 /
    if (read_size > used_size)
    {
        read_size = used_size;
    }
    
    /* 如果指定读的大小小于【读index到缓冲区末尾】的空间大小，则直接读;
     *  否则，分两段读，先读【读index到缓冲区末尾】的空间大小的数据，然后
     *  再从缓冲区开头接着读剩余的数据
     */ 
    copy_size = min(read_size, rb->rb_size - rb->rb_read);
    memcpy(data, rb->prb_buf+rb->rb_read, copy_size);
    memcpy(data+copy_size, rb->prb_buf, read_size-copy_size);
    
    
    /* 更新读index */
    rb->rb_read =  (rb->rb_read + read_size) % rb->rb_size;
    

    return read_size; /* 返回实际读的大小 */

}

/*****************************************************************************
 * 函  数:    util_ringbuffer_get                                                          
 * 功  能:    从环形缓冲区中读取一个字节的数据                                                              
 * 输  入:    *rb        : 指向创建的环形缓冲区  
 * 输  出:                                                       
 * 返回值:    所读的一个字节的数据                                                   
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
//u8_t util_ringbuffer_get(ringbuffer_t *rb)
//{
//    u8_t ch;
//    u16_t used_size = 0;
//    
//    /* 取得可读数据大小 */
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
 * 函  数:    util_ringbuffer_write                                                         
 * 功  能:    向环形缓冲区中写入指定大小的数据                                                               
 * 输  入:    *rb         : 指向创建的环形缓冲区  
 *            *data       : 写数据地址
 *            *write_size : 指定写的大小
 * 输  出:                                                       
 * 返回值:    实际写大小                                                   
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
u16_t util_ringbuffer_write(ringbuffer_t *rb, u8_t *data, u16_t write_size)
{
   
    u16_t no_use_size = 0;
    u16_t copy_size = 0;
    
    
    /* 取得可写空间大小 */
    no_use_size = util_ringbuffer_no_use(rb);
    
    /* 没有空间可写，直接返回 */
    if (no_use_size == 0)
    {
        return 0;
    }
    
    /* 超过可写空间大小，则只写可写空间大小的数据 */
    if (write_size > no_use_size)
    {
        write_size = no_use_size;

    }
    
    /* 如果指定写的大小小于【写index到缓冲区末尾】的空间大小，则直接写;
     *  否则，分两段写，先写【写index到缓冲区末尾】的空间大小的数据，然后
     *  再从缓冲区开头接着写剩余的数据
     */      
    copy_size = min(write_size, rb->rb_size - rb->rb_write);    
    memcpy(rb->prb_buf + rb->rb_write, data, copy_size);
    memcpy(rb->prb_buf, data+copy_size, write_size-copy_size);
    
    /* 更新写index */
    rb->rb_write = (rb->rb_write + write_size) %  rb->rb_size;
    
    
    return write_size; /* 返回实际写的大小 */
}

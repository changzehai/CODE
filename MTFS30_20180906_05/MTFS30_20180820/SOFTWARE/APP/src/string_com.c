/*****************************************************************************/
/* 文件名:    string_com.c                                                       */
/* 描  述:    字符串相关处理                                                 */
/* 创  建:    2018-07-20 changzehai(DTT)                                     */
/* 更  新:    无                                                             */
/* Copyright 1998 - 2018 DTT. All Rights Reserved                            */
/*****************************************************************************/
#include "string_com.h"
#include "mtfs30_debug.h"

/*****************************************************************************
 * 函  数:    str_to_int32u                                                           
 * 功  能:    将字符串转换为无符号整数                                                                 
 * 输  入:    *str: 要转换的字符串                       
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
u32_t str_to_int32u(u8_t *str)
{

	u8_t    i = 0;
	u32_t sum = 0;

	for (i = 0; i < strlen((char *)str); i++)
	{
		sum = (sum << 1) + (sum << 3) + (str[i] & 0xf);
	}		

	return sum;	 
}

/*****************************************************************************
 * 函  数:    str_to_int32s                                                           
 * 功  能:    将字符串转换为有符号整数                                                                 
 * 输  入:    *str: 要转换的字符串                       
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
s32_t str_to_int32s(u8_t *str)
{

	u8_t    i = 0;
	s8_t     symbol = 1;
	s32_t    sum = 0;



	if (str[0] == '-') 
	{
		symbol = -1;
		i = 1;
	}

	for (i; i < strlen((char *)str); i++)
	{
		sum = (sum << 1) + (sum << 3) + (str[i] & 0xf);
	}

	sum = sum * symbol;	

	return sum;	 
}


/*****************************************************************************
 * 函  数:    my_strtok                                                           
 * 功  能:    截取字符串中指定字符前的部分                                                                
 * 输  入:    ch  : 指定字符 
 *            *str: 只要要截取的字符串
 * 输  出:    *sub_str: 指向截取后的子串                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
u8_t  my_strtok(char  ch, const char *str, char *sub_str, u16_t sub_str_size)
{

    u8_t sub_str_len = 0;
    
    /* 输入参数检查 */
    if (str == NULL || sub_str == NULL)
    {
        return 0;
    }
    
   
    while(*str)
    {

        /* 取到指定字符，截取完毕 */
        if (*str == ch)
        {
            sub_str[sub_str_len] = '\0';
            break;
        }
 

        /* 字符拷贝 */
        *sub_str++ = *str++;
//#ifdef DEBUG_MTFS30
//                    MTFS30_DEBUG("%c\n", sub_str[sub_str_len]);  
//#endif        
        
        /* 子串长度加1 */
        sub_str_len ++ ;
        if (sub_str_len == sub_str_size - 1)
        {
            sub_str[sub_str_len] = '\0';
            break;
        }
         
    }
    
    
    return sub_str_len;
    
}


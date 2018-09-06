#ifndef __DHT11_H__
#define __HDT11_H__
//设定标志(static unsigned char status)的宏值
#define OK 1
#define ERROR 0

//函数的返回值表示读取数据是否成功 OK 表示成功 ERROR 表示失败
extern uint8_t Read_DHT11(void);
extern void DHT11_GPIO_Config(void);
#endif
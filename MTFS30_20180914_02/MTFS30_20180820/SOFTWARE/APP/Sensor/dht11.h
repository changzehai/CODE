#ifndef __DHT11_H__
#define __HDT11_H__
//�趨��־(static unsigned char status)�ĺ�ֵ
#define OK 1
#define ERROR 0

//�����ķ���ֵ��ʾ��ȡ�����Ƿ�ɹ� OK ��ʾ�ɹ� ERROR ��ʾʧ��
extern uint8_t Read_DHT11(void);
extern void DHT11_GPIO_Config(void);
#endif
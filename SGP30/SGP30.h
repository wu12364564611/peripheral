#ifndef _SGP30_H
#define _SGP30_H
#include "headfile.h"
#define  SGP30_SCL   PBout(0)
#define  SGP30_SDA   PBout(1)
#define  SGP30_SDA_READ()           GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1)

#define SGP30_read  0xb1  //SGP30的读地址
#define SGP30_write 0xb0  //SGP30的写地址


void SGP30_IIC_Init(void);
void SGP30_Read(uint32_t *array);
void SGP30_Write(uint8_t a,uint8_t b);

#endif



#ifndef _DHT11_H
#define _DHT11_H
#include "headfile.h"

#define RCC_GPIO_DHT11 RCC_APB2Periph_GPIOE
#define GPIO_DHT11 GPIOE
#define GPIO_PIN_DHT11 GPIO_Pin_0
void DHT11_GPIO_Init(void);
void DHT11_Read_Data(uint8_t *array);   
 

#endif




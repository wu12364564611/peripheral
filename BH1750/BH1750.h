#ifndef _BH1750_H
#define _BH1750_H
#include "headfile.h"
#define  BH1750_SCL   PDout(6)
#define  BH1750_SDA   PDout(7)
#define  BH1750_SDA_READ()        GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_7)

#define BH1750_Write     0x46     
#define BH1750_Read      0x47      
#define BH1750_PowDown      0x00      
#define BH1750_PowOn        0x01      
#define BH1750_Reset        0x07      
#define BH1750_ModeH1       0x10      
#define BH1750_ModeH2       0x11      
#define BH1750_ModeL        0x13      
#define BH1750_SigModeH     0x20      
#define BH1750_SigModeH2    0x21     
#define BH1750_SigModeL     0x23

uint16_t BH1750_IIC_Read(void);
void BH1750_IIC_Init(void);
void BH1750_SET(uint8_t command);


#endif


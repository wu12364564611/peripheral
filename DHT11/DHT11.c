#include "DHT11.h"

void DHT11_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_GPIO_DHT11,ENABLE);
 
	GPIO_InitStructure.GPIO_Pin=GPIO_PIN_DHT11;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIO_DHT11,&GPIO_InitStructure);
	GPIO_SetBits(GPIO_DHT11,GPIO_PIN_DHT11);	   
}

void DHT11_IO_OUT(void)	
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin=GPIO_PIN_DHT11;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;	 
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIO_DHT11,&GPIO_InitStructure);	
}
 
void DHT11_IO_IN(void)	
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin=GPIO_PIN_DHT11;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;	
	GPIO_Init(GPIO_DHT11,&GPIO_InitStructure);	
}



void DHT11_Start(void)	   
{                 
	DHT11_IO_OUT(); 		
	GPIO_ResetBits(GPIO_DHT11,GPIO_PIN_DHT11);
	SysTick_Delay_Ms(20);    		
	GPIO_SetBits(GPIO_DHT11,GPIO_PIN_DHT11); 		
	SysTick_Delay_Us(30);     	
}


uint8_t DHT11_Check() 	   
{   
	uint8_t retry=0;
	DHT11_IO_IN();
  while(GPIO_ReadInputDataBit(GPIO_DHT11,GPIO_PIN_DHT11)&&retry<100)
	{
		retry++;
		SysTick_Delay_Us(1);
	} 
	if(retry>=100)return 1;
	else retry=0;
  while (!GPIO_ReadInputDataBit(GPIO_DHT11,GPIO_PIN_DHT11)&&retry<100)
	{
		retry++;
		SysTick_Delay_Us(1);
	}
	if(retry>=100)return 1;	    
	return 0;
}
 
 
uint8_t DHT11_Read_Bit(void) 			 
{
 	uint8_t retry=0;
	while(GPIO_ReadInputDataBit(GPIO_DHT11,GPIO_PIN_DHT11)&&retry<100)
	{
		retry++;
		SysTick_Delay_Us(1);
	}
	retry=0;
	while(!GPIO_ReadInputDataBit(GPIO_DHT11,GPIO_PIN_DHT11)&&retry<100)
	{
		retry++;
		SysTick_Delay_Us(1);
	}
	SysTick_Delay_Us(40);
	if(GPIO_ReadInputDataBit(GPIO_DHT11,GPIO_PIN_DHT11))return 1;
	else return 0;		   
}

uint8_t DHT11_Read_Byte(void)    
{        
  uint8_t i,dat;
  dat=0;
	for (i=0;i<8;i++) 
	{
		dat<<=1; 
		dat|=DHT11_Read_Bit();
  }						    
  return dat;
}

void DHT11_Read_Data(uint8_t *array)    
{        
 	uint8_t buf[5];
	uint8_t i;
	DHT11_Start();
	if(DHT11_Check()==0)
	{
		for(i=0;i<5;i++)
		{
			buf[i]=DHT11_Read_Byte();
		}
		if((buf[0]+buf[1]+buf[2]+buf[3])==buf[4])
		{
			array[0]=buf[0];
			array[1]=buf[2];
		}
		
	}    
}





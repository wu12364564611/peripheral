#include "SGP30.h"

/*初始化SGP30 IIC的GPIO*/
void SGP30_IIC_Init(void)
{					     
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	//SCL PB0 SDA PB1
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure); 
	SGP30_SCL=1;
	SGP30_SDA=1;
	SGP30_Write(0x20,0x03);
}

/*将SGP30 IIC的SDA设置为输出模式*/
void SGP30_SDA_OUT(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
}

/*将SGP30 IIC的SDA设置为输入模式*/
void SGP30_SDA_IN(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
}


/*发出起始信号*/
void SGP30_IIC_Start(void)
{
	SGP30_SDA_OUT();
	SGP30_SDA=1;	  	  
	SGP30_SCL=1;
	SysTick_Delay_Us(4);
 	SGP30_SDA=0;//START:当 CLK 为高电平时,DATA 从高到低改变
	SysTick_Delay_Us(4);	
	SGP30_SCL=0;//钳住I2C总线，准备发送或接收数据 
	SysTick_Delay_Us(4);
}

/*发出终止信号*/
void SGP30_IIC_Stop(void)
{	
	SGP30_SDA_OUT();
	SGP30_SDA=0;//STOP:当 CLK 为高电平时候， SDA出现一个上调表示IIC总线停止信号
	SGP30_SCL=1;
 	SysTick_Delay_Us(4);
	SGP30_SDA=1;//发送I2C总线结束信号						   	
}


/*等待应答信号*/
//返回值：1，接收应答失败  0，接收应答成功
uint8_t SGP30_IIC_Wait_Ack(void)
{
	
	uint8_t re;
	SGP30_SDA_IN();
	SGP30_SDA=1;SysTick_Delay_Us(1);//CPU释放SDA总线	   
	SGP30_SCL=1;SysTick_Delay_Us(1);//CPU驱动SCL=1,此时器件会返回ACK应答
	if(SGP30_SDA_READ())//CPU读取SDA口线状态
	{
	  re=1;
	}
	else
	{
	  re=0;
	}	
	SGP30_SCL=0;//时钟输出0 
	return re;  
} 


/*CPU产生一个ACK信号*/
void SGP30_IIC_Ack(void)
{
	SGP30_SDA_OUT();
	SGP30_SDA=0;//CPU驱动SDA=0
	SysTick_Delay_Us(2);
	SGP30_SCL=1;//CPU产生一个时钟
	SysTick_Delay_Us(2);
	SGP30_SCL=0;
	SysTick_Delay_Us(2);
	SGP30_SDA=1;//CPU释放SDA总线
}


/*CPU产生1个NACK信号*/
void SGP30_IIC_Nack (void)
{
	SGP30_SDA_OUT();
	SGP30_SDA=1;//CPU驱动SDA=1
	SysTick_Delay_Us(2);
	SGP30_SDA=1;//CPU产生1个时钟
	SysTick_Delay_Us(2);
	SGP30_SCL=0;
	SysTick_Delay_Us(2);	
}

/*发送一个字节*/
void SGP30_IIC_SendByte(uint8_t Byte)
{
	
	uint8_t i;
	SGP30_SDA_OUT();
	/* 先发送字节的高位bit7 */
	for (i = 0; i < 8; i++)
	{		
		if (Byte & 0x80)
		{
		  SGP30_SDA=1;
		}
		else
		{
			SGP30_SDA=0;
		}
		SysTick_Delay_Us(2);
		SGP30_SCL=1;
		SysTick_Delay_Us(2);
		SGP30_SCL=0;
		if (i == 7)
		{
			 SGP30_SDA=1;// 释放总线
		}
		Byte <<= 1;	/* 左移一个bit */
		SysTick_Delay_Us(2);
	}
}


/*读取一个字节*/
uint8_t SGP30_IIC_ReadByte(void)
{
	uint8_t i;
	uint8_t value;
	SGP30_SDA_IN();
	//读到第1个bit为数据的bit7
	value = 0;
	for (i = 0; i < 8; i++)
	{
		value <<= 1;
		SGP30_SCL=1;
		SysTick_Delay_Us(2);
		if (SGP30_SDA_READ())
		{
			value++;
		}
		SGP30_SCL=0;
		SysTick_Delay_Us(2);
	}
	return value;
}

void SGP30_Write(uint8_t a,uint8_t b)
{
	SGP30_IIC_Start();
	SGP30_IIC_SendByte(SGP30_write);
	SGP30_IIC_Wait_Ack();
	SGP30_IIC_SendByte(a);
	SGP30_IIC_Wait_Ack();
	SGP30_IIC_SendByte(b);
	SGP30_IIC_Wait_Ack();
	SGP30_IIC_Stop();
	SysTick_Delay_Ms(100);

}

/*读取SGP30的数据*/
void SGP30_Read(uint32_t *array)
{
	uint32_t dat;
	uint8_t crc;
	SGP30_Write(0x20,0x08);
	//发送起始位
	SGP30_IIC_Start();
	//发送器件地址+读指令
	SGP30_IIC_SendByte(SGP30_read);
	//等待应答
	SGP30_IIC_Wait_Ack();
	
	//读完一个字节产生一次应答
	dat=SGP30_IIC_ReadByte();
	SGP30_IIC_Ack();
	dat<<=8;
	dat += SGP30_IIC_ReadByte();
	SGP30_IIC_Ack();
	crc=SGP30_IIC_ReadByte();
	SGP30_IIC_Ack();
	crc=crc;
	dat<<=8;
	dat += SGP30_IIC_ReadByte();
	SGP30_IIC_Ack();
	dat<<=8;
	dat += SGP30_IIC_ReadByte();
	SGP30_IIC_Nack();
	//发送结束信号
	SGP30_IIC_Stop();
	
	array[0]=(dat&0xffff0000) >> 16;
	array[1]=dat&0x0000ffff;
}


#include "BH1750.h"
/*初始化BH1750 IIC的GPIO*/
void BH1750_IIC_Init(void)
{					     
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
	//SCL PD6 SDA PD7
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD,&GPIO_InitStructure); 
	BH1750_SCL=1;
	BH1750_SDA=1;
	BH1750_SET(BH1750_PowOn);
	BH1750_SET(BH1750_Reset);
	BH1750_SET(BH1750_ModeL);
	SysTick_Delay_Ms(180);

}

/*将BH1750 IIC的SDA设置为输出模式*/
void BH1750_SDA_OUT(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD,&GPIO_InitStructure);
}

/*将BH1750 IIC的SDA设置为输入模式*/
void BH1750_SDA_IN(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD,&GPIO_InitStructure);
}


/*发出起始信号*/
void BH1750_IIC_Start(void)
{
	BH1750_SDA_OUT();
	BH1750_SDA=1;	  	  
	BH1750_SCL=1;
	SysTick_Delay_Us(4);
 	BH1750_SDA=0;//START:当 CLK 为高电平时,DATA 从高到低改变
	SysTick_Delay_Us(4);	
	BH1750_SCL=0;//钳住I2C总线，准备发送或接收数据 
	SysTick_Delay_Us(4);
}

/*发出终止信号*/
void BH1750_IIC_Stop(void)
{	
	BH1750_SDA_OUT();
	BH1750_SDA=0;//STOP:当 CLK 为高电平时候， SDA出现一个上调表示IIC总线停止信号
	BH1750_SCL=1;
 	SysTick_Delay_Us(4);
	BH1750_SDA=1;//发送I2C总线结束信号						   	
}


/*等待应答信号*/
//返回值：1，接收应答失败  0，接收应答成功
uint8_t BH1750_IIC_Wait_Ack(void)
{
	
	uint8_t re;
	BH1750_SDA_IN();
	BH1750_SDA=1;SysTick_Delay_Us(1);//CPU释放SDA总线	   
	BH1750_SCL=1;SysTick_Delay_Us(1);//CPU驱动SCL=1,此时器件会返回ACK应答
	if(BH1750_SDA_READ())//CPU读取SDA口线状态
	{
	  re=1;
	}
	else
	{
	  re=0;
	}	
	BH1750_SCL=0;//时钟输出0 	
	return re;  
} 


/*CPU产生一个ACK信号*/
void BH1750_IIC_Ack(void)
{
	BH1750_SDA_OUT();
	BH1750_SDA=0;//CPU驱动SDA=0
	SysTick_Delay_Us(2);
	BH1750_SCL=1;//CPU产生一个时钟
	SysTick_Delay_Us(2);
	BH1750_SCL=0;
	SysTick_Delay_Us(2);
	BH1750_SDA=1;//CPU释放SDA总线
}


/*CPU产生1个NACK信号*/
void BH1750_IIC_Nack (void)
{
	BH1750_SDA_OUT();
	BH1750_SDA=1;//CPU驱动SDA=1
	SysTick_Delay_Us(2);
	BH1750_SDA=1;//CPU产生1个时钟
	SysTick_Delay_Us(2);
	BH1750_SCL=0;
	SysTick_Delay_Us(2);	
}

/*发送一个字节*/
void BH1750_IIC_SendByte(uint8_t Byte)
{
	
	uint8_t i;
	BH1750_SDA_OUT();
	/* 先发送字节的高位bit7 */
	for (i = 0; i < 8; i++)
	{		
		if (Byte & 0x80)
		{
		  BH1750_SDA=1;
		}
		else
		{
			BH1750_SDA=0;
		}
		SysTick_Delay_Us(2);
		BH1750_SCL=1;
		SysTick_Delay_Us(2);
		BH1750_SCL=0;
		if (i == 7)
		{
			 BH1750_SDA=1;// 释放总线
		}
		Byte <<= 1;	/* 左移一个bit */
		SysTick_Delay_Us(2);
	}
}


/*读取一个字节*/
uint8_t BH1750_IIC_ReadByte(void)
{
	uint8_t i;
	uint8_t value;
	BH1750_SDA_IN();
	//读到第1个bit为数据的bit7
	value = 0;
	for (i = 0; i < 8; i++)
	{
		value <<= 1;
		BH1750_SCL=1;
		SysTick_Delay_Us(2);
		if (BH1750_SDA_READ())
		{
			value++;
		}
		BH1750_SCL=0;
		SysTick_Delay_Us(2);
	}
	return value;
}

void BH1750_SET(uint8_t command)
{
	BH1750_IIC_Start();
	//发送器件地址+读指令
	BH1750_IIC_SendByte(BH1750_Write);
	//等待应答
	BH1750_IIC_Wait_Ack();
	//读完一个字节产生一次应答
	BH1750_IIC_SendByte(command);
	BH1750_IIC_Wait_Ack();
	//发送结束信号
	BH1750_IIC_Stop();

}

/*读取BH1750的数据*/
uint16_t BH1750_IIC_Read(void)
{
	uint16_t buff;
	//发送起始位
	BH1750_IIC_Start();
	//发送器件地址+读指令
	BH1750_IIC_SendByte(BH1750_Read);
	//等待应答
	BH1750_IIC_Wait_Ack();
	//读完一个字节产生一次应答
	buff=BH1750_IIC_ReadByte();
	buff<<=8;
	BH1750_IIC_Ack();
	buff+=0x00ff&BH1750_IIC_ReadByte();
	BH1750_IIC_Nack();
	//发送结束信号
	BH1750_IIC_Stop();
	
	return buff;
}

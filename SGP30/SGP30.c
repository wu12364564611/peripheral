#include "SGP30.h"

/*��ʼ��SGP30 IIC��GPIO*/
void SGP30_IIC_Init(void)
{					     
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	//SCL PB0 SDA PB1
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure); 
	SGP30_SCL=1;
	SGP30_SDA=1;
	SGP30_Write(0x20,0x03);
}

/*��SGP30 IIC��SDA����Ϊ���ģʽ*/
void SGP30_SDA_OUT(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
}

/*��SGP30 IIC��SDA����Ϊ����ģʽ*/
void SGP30_SDA_IN(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
}


/*������ʼ�ź�*/
void SGP30_IIC_Start(void)
{
	SGP30_SDA_OUT();
	SGP30_SDA=1;	  	  
	SGP30_SCL=1;
	SysTick_Delay_Us(4);
 	SGP30_SDA=0;//START:�� CLK Ϊ�ߵ�ƽʱ,DATA �Ӹߵ��͸ı�
	SysTick_Delay_Us(4);	
	SGP30_SCL=0;//ǯסI2C���ߣ�׼�����ͻ�������� 
	SysTick_Delay_Us(4);
}

/*������ֹ�ź�*/
void SGP30_IIC_Stop(void)
{	
	SGP30_SDA_OUT();
	SGP30_SDA=0;//STOP:�� CLK Ϊ�ߵ�ƽʱ�� SDA����һ���ϵ���ʾIIC����ֹͣ�ź�
	SGP30_SCL=1;
 	SysTick_Delay_Us(4);
	SGP30_SDA=1;//����I2C���߽����ź�						   	
}


/*�ȴ�Ӧ���ź�*/
//����ֵ��1������Ӧ��ʧ��  0������Ӧ��ɹ�
uint8_t SGP30_IIC_Wait_Ack(void)
{
	
	uint8_t re;
	SGP30_SDA_IN();
	SGP30_SDA=1;SysTick_Delay_Us(1);//CPU�ͷ�SDA����	   
	SGP30_SCL=1;SysTick_Delay_Us(1);//CPU����SCL=1,��ʱ�����᷵��ACKӦ��
	if(SGP30_SDA_READ())//CPU��ȡSDA����״̬
	{
	  re=1;
	}
	else
	{
	  re=0;
	}	
	SGP30_SCL=0;//ʱ�����0 
	return re;  
} 


/*CPU����һ��ACK�ź�*/
void SGP30_IIC_Ack(void)
{
	SGP30_SDA_OUT();
	SGP30_SDA=0;//CPU����SDA=0
	SysTick_Delay_Us(2);
	SGP30_SCL=1;//CPU����һ��ʱ��
	SysTick_Delay_Us(2);
	SGP30_SCL=0;
	SysTick_Delay_Us(2);
	SGP30_SDA=1;//CPU�ͷ�SDA����
}


/*CPU����1��NACK�ź�*/
void SGP30_IIC_Nack (void)
{
	SGP30_SDA_OUT();
	SGP30_SDA=1;//CPU����SDA=1
	SysTick_Delay_Us(2);
	SGP30_SDA=1;//CPU����1��ʱ��
	SysTick_Delay_Us(2);
	SGP30_SCL=0;
	SysTick_Delay_Us(2);	
}

/*����һ���ֽ�*/
void SGP30_IIC_SendByte(uint8_t Byte)
{
	
	uint8_t i;
	SGP30_SDA_OUT();
	/* �ȷ����ֽڵĸ�λbit7 */
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
			 SGP30_SDA=1;// �ͷ�����
		}
		Byte <<= 1;	/* ����һ��bit */
		SysTick_Delay_Us(2);
	}
}


/*��ȡһ���ֽ�*/
uint8_t SGP30_IIC_ReadByte(void)
{
	uint8_t i;
	uint8_t value;
	SGP30_SDA_IN();
	//������1��bitΪ���ݵ�bit7
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

/*��ȡSGP30������*/
void SGP30_Read(uint32_t *array)
{
	uint32_t dat;
	uint8_t crc;
	SGP30_Write(0x20,0x08);
	//������ʼλ
	SGP30_IIC_Start();
	//����������ַ+��ָ��
	SGP30_IIC_SendByte(SGP30_read);
	//�ȴ�Ӧ��
	SGP30_IIC_Wait_Ack();
	
	//����һ���ֽڲ���һ��Ӧ��
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
	//���ͽ����ź�
	SGP30_IIC_Stop();
	
	array[0]=(dat&0xffff0000) >> 16;
	array[1]=dat&0x0000ffff;
}


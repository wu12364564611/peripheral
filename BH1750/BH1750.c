#include "BH1750.h"
/*��ʼ��BH1750 IIC��GPIO*/
void BH1750_IIC_Init(void)
{					     
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
	//SCL PD6 SDA PD7
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD,&GPIO_InitStructure); 
	BH1750_SCL=1;
	BH1750_SDA=1;
	BH1750_SET(BH1750_PowOn);
	BH1750_SET(BH1750_Reset);
	BH1750_SET(BH1750_ModeL);
	SysTick_Delay_Ms(180);

}

/*��BH1750 IIC��SDA����Ϊ���ģʽ*/
void BH1750_SDA_OUT(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD,&GPIO_InitStructure);
}

/*��BH1750 IIC��SDA����Ϊ����ģʽ*/
void BH1750_SDA_IN(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD,&GPIO_InitStructure);
}


/*������ʼ�ź�*/
void BH1750_IIC_Start(void)
{
	BH1750_SDA_OUT();
	BH1750_SDA=1;	  	  
	BH1750_SCL=1;
	SysTick_Delay_Us(4);
 	BH1750_SDA=0;//START:�� CLK Ϊ�ߵ�ƽʱ,DATA �Ӹߵ��͸ı�
	SysTick_Delay_Us(4);	
	BH1750_SCL=0;//ǯסI2C���ߣ�׼�����ͻ�������� 
	SysTick_Delay_Us(4);
}

/*������ֹ�ź�*/
void BH1750_IIC_Stop(void)
{	
	BH1750_SDA_OUT();
	BH1750_SDA=0;//STOP:�� CLK Ϊ�ߵ�ƽʱ�� SDA����һ���ϵ���ʾIIC����ֹͣ�ź�
	BH1750_SCL=1;
 	SysTick_Delay_Us(4);
	BH1750_SDA=1;//����I2C���߽����ź�						   	
}


/*�ȴ�Ӧ���ź�*/
//����ֵ��1������Ӧ��ʧ��  0������Ӧ��ɹ�
uint8_t BH1750_IIC_Wait_Ack(void)
{
	
	uint8_t re;
	BH1750_SDA_IN();
	BH1750_SDA=1;SysTick_Delay_Us(1);//CPU�ͷ�SDA����	   
	BH1750_SCL=1;SysTick_Delay_Us(1);//CPU����SCL=1,��ʱ�����᷵��ACKӦ��
	if(BH1750_SDA_READ())//CPU��ȡSDA����״̬
	{
	  re=1;
	}
	else
	{
	  re=0;
	}	
	BH1750_SCL=0;//ʱ�����0 	
	return re;  
} 


/*CPU����һ��ACK�ź�*/
void BH1750_IIC_Ack(void)
{
	BH1750_SDA_OUT();
	BH1750_SDA=0;//CPU����SDA=0
	SysTick_Delay_Us(2);
	BH1750_SCL=1;//CPU����һ��ʱ��
	SysTick_Delay_Us(2);
	BH1750_SCL=0;
	SysTick_Delay_Us(2);
	BH1750_SDA=1;//CPU�ͷ�SDA����
}


/*CPU����1��NACK�ź�*/
void BH1750_IIC_Nack (void)
{
	BH1750_SDA_OUT();
	BH1750_SDA=1;//CPU����SDA=1
	SysTick_Delay_Us(2);
	BH1750_SDA=1;//CPU����1��ʱ��
	SysTick_Delay_Us(2);
	BH1750_SCL=0;
	SysTick_Delay_Us(2);	
}

/*����һ���ֽ�*/
void BH1750_IIC_SendByte(uint8_t Byte)
{
	
	uint8_t i;
	BH1750_SDA_OUT();
	/* �ȷ����ֽڵĸ�λbit7 */
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
			 BH1750_SDA=1;// �ͷ�����
		}
		Byte <<= 1;	/* ����һ��bit */
		SysTick_Delay_Us(2);
	}
}


/*��ȡһ���ֽ�*/
uint8_t BH1750_IIC_ReadByte(void)
{
	uint8_t i;
	uint8_t value;
	BH1750_SDA_IN();
	//������1��bitΪ���ݵ�bit7
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
	//����������ַ+��ָ��
	BH1750_IIC_SendByte(BH1750_Write);
	//�ȴ�Ӧ��
	BH1750_IIC_Wait_Ack();
	//����һ���ֽڲ���һ��Ӧ��
	BH1750_IIC_SendByte(command);
	BH1750_IIC_Wait_Ack();
	//���ͽ����ź�
	BH1750_IIC_Stop();

}

/*��ȡBH1750������*/
uint16_t BH1750_IIC_Read(void)
{
	uint16_t buff;
	//������ʼλ
	BH1750_IIC_Start();
	//����������ַ+��ָ��
	BH1750_IIC_SendByte(BH1750_Read);
	//�ȴ�Ӧ��
	BH1750_IIC_Wait_Ack();
	//����һ���ֽڲ���һ��Ӧ��
	buff=BH1750_IIC_ReadByte();
	buff<<=8;
	BH1750_IIC_Ack();
	buff+=0x00ff&BH1750_IIC_ReadByte();
	BH1750_IIC_Nack();
	//���ͽ����ź�
	BH1750_IIC_Stop();
	
	return buff;
}

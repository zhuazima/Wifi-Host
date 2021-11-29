#include "hal_eeprom.h"
#include "stm32f10x.h"

static void hal_EepromConfig(void);
static void hal_I2C_SDA(unsigned char bVal);
static void hal_I2C_SCL(unsigned char bVal);

static void I2C_delay(unsigned char t)
{
	unsigned char i;
	while(t--)
	{
		i = 50;
		while(i--);
	}
} 

static void I2C_Start(void)
{
	hal_I2C_SDA(1);
	I2C_delay(1);
	hal_I2C_SCL(1);
	I2C_delay(1);
	hal_I2C_SDA(0);
	I2C_delay(1);

}

static void I2C_Stop(void)
{
	hal_I2C_SDA(0);
	I2C_delay(1);
	hal_I2C_SCL(0);
	I2C_delay(1);
	hal_I2C_SDA(1);
	I2C_delay(1);

}

void hal_EepromInit(void)
{
	hal_EepromConfig();
}

static void hal_EepromConfig(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure; 
	/* Configure I2C2 pins: PB8->SCL and PB9->SDA */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitStructure.GPIO_Pin =  I2C_SCL_PIN | I2C_SDA_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;  
	GPIO_Init(I2C_SCL_PORT, &GPIO_InitStructure);

	hal_I2C_SDA(1);
	hal_I2C_SCL(1);
}

static void hal_I2C_SDA(unsigned char bVal)
{
	if(bVal)
	{
		GPIO_SetBits(I2C_SDA_PORT,I2C_SDA_PIN);
	}
	else
	{
		GPIO_ResetBits(I2C_SDA_PORT,I2C_SDA_PIN);

	}
}

static void hal_I2C_SCL(unsigned char bVal)
{
	if(bVal)
	{
		GPIO_SetBits(I2C_SCL_PORT,I2C_SCL_PIN);
	}
	else
	{
		GPIO_ResetBits(I2C_SCL_PORT,I2C_SCL_PIN);

	}
}
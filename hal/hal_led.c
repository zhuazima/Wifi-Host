#include "stm32f10x.h"
#include "hal_led.h"

static void hal_ledConfig(void);

void hal_LedInit(void)
{
	hal_ledConfig();
	
}

void hal_LedProc(void)
{
	
	static unsigned short i = 0;
	i++;
	if(i > 100)
	{
		i=0;
		hal_LedTurn();
	}
}


static void hal_ledConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA , ENABLE); 						 
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB , ENABLE); 	


	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; ; 
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOA,GPIO_Pin_1);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; ; 
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOB,GPIO_Pin_0);	
	
}


void hal_Led1Drive(unsigned char sta)
{
	if(sta)
	{
		GPIO_SetBits(GPIOA,GPIO_Pin_1);
	}else
	{
		GPIO_ResetBits(GPIOA,GPIO_Pin_1);
	}
}


void hal_LedTurn(void)
{
	GPIO_WriteBit(GPIOA,GPIO_Pin_1,(BitAction)(1-GPIO_ReadOutputDataBit(GPIOA,GPIO_Pin_1)));
}

void hal_BuzDrive(unsigned char sta)
{
	if(sta)
	{
		GPIO_SetBits(BUZ_PORT,BUZ_PIN);
	}else
	{
		GPIO_ResetBits(BUZ_PORT,BUZ_PIN);
	}
}



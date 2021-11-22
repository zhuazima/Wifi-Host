/*
 * @Author: your name
 * @Date: 2021-11-20 19:28:49
 * @LastEditTime: 2021-11-22 19:01:18
 * @LastEditors: Please set LastEditors
 * @Description: 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 * @FilePath: \Wifi Host\hal\hal_led.c
 */
#include "stm32f10x.h"
#include "hal_led.h"
#include "OS_System.h"


Queue4 	LedCmdBuff[LED_SUM];		//LED信箱
unsigned char LedLoadFlag[LED_SUM];


unsigned short Led_Dark[] = {0,10,LED_EFFECT_END};
unsigned short Led_Light[] = {1,10,LED_EFFECT_END};
unsigned short Led_Light100ms[] = {1,10,0,10,LED_EFFECT_END};	
unsigned short Led_Blink1[] = {1,10,0,10,LED_EFFECT_AGN,2};
unsigned short Led_Blink2[] = {1,10,0,10,1,10,0,10,1,10,0,200,LED_EFFECT_AGN,6};
unsigned short Led_Blink3[] = {1,30,0,30,LED_EFFECT_AGN,2};
unsigned short Led_Blink4[] = {1,50,0,50,LED_EFFECT_AGN,2};





static void hal_ledConfig(void);

void hal_LedInit(void)
{
	hal_ledConfig();
	
}

void LedMsgInput(unsigned char type,LED_EFFECT_TEPEDEF cmd,unsigned char clr)
{	 
	unsigned char bLedCMD;
	if(type >= LED_SUM)
	{
		return;
	}
	bLedCMD = cmd;
	if(clr)
	{
		QueueEmpty(LedCmdBuff[type]);
		LedLoadFlag[type] = 0;
		 
	}
	QueueDataIn(LedCmdBuff[type],&bLedCMD,1);
	
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



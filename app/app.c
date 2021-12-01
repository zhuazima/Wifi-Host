/*
 * @Author: your name
 * @Date: 2021-11-22 18:39:10
 * @LastEditTime: 2021-11-22 19:20:45
 * @LastEditors: Please set LastEditors
 * @Description: 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 * @FilePath: \Wifi Host\app\app.c
 */
#include "hal_led.h"
#include "hal_key.h"
#include "hal_oled.h"
#include "app.h"
#include "hal_Uart.h"
#include "OS_System.h"
#include "hal_rfd.h"
#include "hal_eeprom.h"
#include "para.h"


Queue8 RFDRcvMsg;	//RFD接收队列


static void KeyEventHandle(KEY_VALUE_TYPEDEF keys);
static void RfdRcvHandle(unsigned char *pBuff);

void AppInit(void)
{	
	unsigned char Write[66],Read[66],i;
	
	hal_Oled_Init();
	hal_EepromInit();
	Para_Init();
	QueueEmpty(RFDRcvMsg);
	hal_KeyScanCBSRegister(KeyEventHandle);
	hal_RFCRcvCBSRegister(RfdRcvHandle); 

	hal_Oled_ShowString(40,2,"Recode",16,1);
	//hal_Oled_ShowString(16,20,"Smart alarm",16,1);
	//hal_Oled_ShowString(40,40,"system",16,1);
	hal_Oled_Refresh();

	for(i = 0;i < 66 ;i++)
	{
		Write[i] = i+1;
		Read[i] = 0;
	}

	I2C_PageWrite(0,Write,66);
	I2C_Read(0,Read,66);
	if( (Read[0] == 1) && (Read[1] == 2) )
	{
		LedMsgInput(LED1,LED_BLINK3,1);
	}

	


}





void AppProc(void)
{
	unsigned char tBuff[3],dat;
	if(QueueDataLen(RFDRcvMsg))
	{
		QueueDataOut(RFDRcvMsg,&dat);
		if(dat == '#')
		{
			QueueDataOut(RFDRcvMsg,&tBuff[2]);	//地址码
			QueueDataOut(RFDRcvMsg,&tBuff[1]);	//地址码
			QueueDataOut(RFDRcvMsg,&tBuff[0]);	//数据码
			
			dat = (tBuff[2]>>4)&0x0F;
			if(dat > 9)
			{
				hal_Oled_ShowChar(20,40,(dat-10)+'A',16,1);
			}else
			{
				hal_Oled_ShowChar(20,40,dat+'0',16,1);
			}
			
			
			dat = tBuff[2]&0x0F;
			if(dat > 9)
			{
				hal_Oled_ShowChar(28,40,(dat-10)+'A',16,1);
			}else
			{
				hal_Oled_ShowChar(28,40,dat+'0',16,1);
			}
			 
			hal_Oled_ShowChar(36,40,' ',16,1);
			 
			dat = (tBuff[1]>>4)&0x0F;
			if(dat > 9)
			{
				hal_Oled_ShowChar(44,40,(dat-10)+'A',16,1);
			}else
			{
				hal_Oled_ShowChar(44,40,dat+'0',16,1);
			}
			
			
			dat = tBuff[1]&0x0F;
			if(dat > 9)
			{
				hal_Oled_ShowChar(52,40,(dat-10)+'A',16,1);
			}else
			{
				hal_Oled_ShowChar(52,40,dat+'0',16,1);
			}
			
			hal_Oled_ShowChar(60,40,' ',16,1);
			 
			dat = (tBuff[0]>>4)&0x0F;
			if(dat > 9)
			{
				hal_Oled_ShowChar(68,40,(dat-10)+'A',16,1);
			}else
			{
				hal_Oled_ShowChar(68,40,dat+'0',16,1);
			}
			
			
			dat = tBuff[0]&0x0F;
			if(dat > 9)
			{
				hal_Oled_ShowChar(76,40,(dat-10)+'A',16,1);
			}else
			{
				hal_Oled_ShowChar(76,40,dat+'0',16,1);
			} 

			hal_Oled_Refresh();
		}
	}
}


//-----------------驱动层回调处理函数------------------------
 
//按键回调函数
static void KeyEventHandle(KEY_VALUE_TYPEDEF keys)
{
	
	 
	if((keys==KEY1_CLICK)
	|| (keys==KEY2_CLICK)
	|| (keys==KEY3_CLICK)
	|| (keys==KEY4_CLICK)
	|| (keys==KEY5_CLICK)
	|| (keys==KEY6_CLICK))
	{
		LedMsgInput(LED1,LED_LIGHT,1);
	}else if((keys==KEY1_CLICK_RELEASE)
	|| (keys==KEY2_CLICK_RELEASE)
	|| (keys==KEY3_CLICK_RELEASE)
	|| (keys==KEY4_CLICK_RELEASE)
	|| (keys==KEY5_CLICK_RELEASE)
	|| (keys==KEY6_CLICK_RELEASE))
	{
		LedMsgInput(LED1,LED_BLINK4,1);
	}else if((keys==KEY1_LONG_PRESS)
	|| (keys==KEY2_LONG_PRESS)
	|| (keys==KEY3_LONG_PRESS)
	|| (keys==KEY4_LONG_PRESS)
	|| (keys==KEY5_LONG_PRESS)
	|| (keys==KEY6_LONG_PRESS))
	{
		LedMsgInput(LED1,LED_DARK,1);
	}
}


//RFD接收回调函数
static void RfdRcvHandle(unsigned char *pBuff)
{
	unsigned char temp;
	temp = '#';
	QueueDataIn(RFDRcvMsg, &temp, 1);
	QueueDataIn(RFDRcvMsg, &pBuff[0], 3);
	
	 
}


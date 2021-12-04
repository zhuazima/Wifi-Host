
#include "hal_led.h"
#include "hal_key.h"
#include "hal_oled.h"
#include "app.h"
#include "hal_Uart.h"
#include "OS_System.h"
#include "hal_rfd.h"
#include "hal_eeprom.h"
#include "para.h"
#include "hal_beep.h"
#include "string.h"


#define MENU_MAP_LEN	10
Queue8 RFDRcvMsg;	//RFD接收队列
LINK_STATE_TYPEDEF link_state;

stu_MENU_TYPEDEF MENU_Map[MENU_MAP_LEN],*pCurrentMENU;
stu_system_time stuSystemtime;		//系统时间


static void KeyEventHandle(KEY_VALUE_TYPEDEF keys);
static void RfdRcvHandle(unsigned char *pBuff);
void Menu_Reset(void);
void Menu_Init(void);

void AppInit(void)
{
	
	hal_Oled_Init();
	hal_EepromInit();
	Para_Init();
	hal_BeepInit();
	Menu_Init();
	QueueEmpty(RFDRcvMsg);
	hal_KeyScanCBSRegister(KeyEventHandle);
	hal_RFCRcvCBSRegister(RfdRcvHandle);

	stuSystemtime.year = 2021;
	stuSystemtime.mon = 12;
	stuSystemtime.day = 04;
	stuSystemtime.hour = 21;
	stuSystemtime.min = 23;
	stuSystemtime.week = 6;

}

static void showSystemTime(void)
{
//2021-04-09 18:10 Sun
	hal_Oled_ShowChar(4,54,(stuSystemtime.year/1000)+'0',8,1);
	hal_Oled_ShowChar(10,54,((stuSystemtime.year%1000)/100)+'0',8,1);
	hal_Oled_ShowChar(16,54,((stuSystemtime.year%1000%100)/10)+'0',8,1);
	hal_Oled_ShowChar(22,54,(stuSystemtime.year%1000%100%10)+'0',8,1);
  
	//hal_Oled_Refresh();
	//4+4*6=28
	hal_Oled_ShowString(28,54,"-",8,1);
	//hal_Oled_Refresh();
	//04
	//28+6=34
 
	hal_Oled_ShowChar(34,54,(stuSystemtime.mon/10)+'0',8,1);
	hal_Oled_ShowChar(40,54,(stuSystemtime.mon%10)+'0',8,1);
	//hal_Oled_Refresh();
	
	//hal_Oled_Refresh();
	///34+2*6=46
	hal_Oled_ShowString(46,54,"-",8,1);
	//hal_Oled_Refresh();
	//46+6=52

	hal_Oled_ShowChar(52,54,(stuSystemtime.day/10)+'0',8,1);
	hal_Oled_ShowChar(58,54,(stuSystemtime.day%10)+'0',8,1);
	
	//hal_Oled_Refresh();
	//52+12=64
	hal_Oled_ShowString(64,54," ",8,1);
	//hal_Oled_Refresh();
	//64+6=70
	
	hal_Oled_ShowChar(70,54,(stuSystemtime.hour/10)+'0',8,1);
	hal_Oled_ShowChar(76,54,(stuSystemtime.hour%10)+'0',8,1);
		
	//hal_Oled_Refresh();
	//70+12=82
	hal_Oled_ShowString(82,54,":",8,1);
	//hal_Oled_Refresh();
	//82+6
 
	hal_Oled_ShowChar(88,54,(stuSystemtime.min/10)+'0',8,1);
	hal_Oled_ShowChar(94,54,(stuSystemtime.min%10)+'0',8,1);
		
	//hal_Oled_Refresh();
	//88+12
	hal_Oled_ShowString(100,54," ",8,1);
	//hal_Oled_Refresh();
	switch(stuSystemtime.week)
	{
		case 1:	//星期1
			hal_Oled_ShowString(106,54,"Mon",8,1);
		break;
		case 2:	//星期2
			hal_Oled_ShowString(106,54,"Tue",8,1);
		break;
		case 3:	//星期3
			hal_Oled_ShowString(106,54,"Wed",8,1);
		break;
		case 4:	//星期4
			hal_Oled_ShowString(106,54,"Thu",8,1);
		break;
		case 5:	//星期5
			hal_Oled_ShowString(106,54,"Fir",8,1);
		break;
		case 6:	//星期6
			hal_Oled_ShowString(106,54,"Sat",8,1);
		break;
		case 7:	//星期7
			hal_Oled_ShowString(106,54,"Sun",8,1);
		break;
		
	}
	hal_Oled_Refresh();
}



void AppProc(void)
{
	
	pCurrentMENU->action();

}



void Desk_Menu_ActionCBS(void)
{
	if(pCurrentMENU ->refreshScreenCmd == SCREEN_CMD_RESET)
	{
		pCurrentMENU ->refreshScreenCmd = SCREEN_CMD_NULL;

		hal_Oled_Clear();
		
		hal_Oled_ShowString(0,0,"w",8,1);	//8 *16
		hal_Oled_ShowString(16,20,"Away arm",24,1); //12*24
		showSystemTime();

		hal_Oled_Refresh();
	}

}

void Menu_Init(void)
{
	Menu_Reset();

	pCurrentMENU = &MENU_Map[0];
	pCurrentMENU->action = Desk_Menu_ActionCBS;
	pCurrentMENU->refreshScreenCmd = SCREEN_CMD_RESET;
}

void Menu_Reset(void)
{
	unsigned char i;
	for(i = 0;i < MENU_MAP_LEN; i++)
	{
		MENU_Map[i].ID = i + 1;
		MENU_Map[i].Name = 0;
		MENU_Map[i].refreshScreenCmd = SCREEN_CMD_NULL;
		MENU_Map[i].action = 0;
		MENU_Map[i].Last_Menu = 0;
		MENU_Map[i].Next_Menu = 0;
		MENU_Map[i].Father_Menu = 0;
		MENU_Map[i].ChiLdren_Menu = 0;
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


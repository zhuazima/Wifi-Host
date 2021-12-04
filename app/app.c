
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


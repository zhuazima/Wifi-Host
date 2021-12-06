
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

stu_system_time stuSystemtime;		//系统时间
stu_mode_menu *pModeMenu;		//系统当前菜单

static void KeyEventHandle(KEY_VALUE_TYPEDEF keys);
static void RfdRcvHandle(unsigned char *pBuff);
void Menu_Reset(void);
void Menu_Init(void);
static void gnlMenu_DesktopCBS(void);
static void stgMenu_MainMenuCBS(void);
static void stgMenu_FactorySettingsCBS(void);
static void stgMenu_MachineInfoCBS(void);
static void stgMenu_WifiCBS(void);
static void stgMenu_DTCListCBS(void);
static void stgMenu_LearnSensorCBS(void);

//初始化桌面菜单
stu_mode_menu generalModeMenu[GNL_MENU_SUM] =
{
	{GNL_MENU_DESKTOP,DESKTOP_MENU_POS,"Desktop",gnlMenu_DesktopCBS,1,0,0xFF,0,0,0,0},
	
};	

//初始化设置菜单
stu_mode_menu settingModeMenu[STG_MENU_SUM] = 
{
	{STG_MENU_MAIN_SETTING,STG_MENU_POS,"Main Menu",stgMenu_MainMenuCBS,1,0,0xFF,0,0,0,0},		//设置主页面
	{STG_MENU_LEARNING_SENSOR,STG_SUB_2_MENU_POS,"1. Learning Dtc",stgMenu_LearnSensorCBS,1,0,0xFF,0,0,0,0},	//探测器学习
	{STG_MENU_DTC_LIST,STG_SUB_2_MENU_POS,"2. Dtc List",stgMenu_DTCListCBS,1,0,0xFF,0,0,0,0},			//防区


	{STG_MENU_WIFI,STG_WIFI_MENU_POS,"3. WiFi",stgMenu_WifiCBS,1,0,0xFF,0,0,0,0},
	//{STG_MENU_TIME,STG_SUB_2_MENU_POS,"4. Time Set",stgMenu_TimeCBS,1,0,0xFF,0,0,0,0},
	{STG_MENU_MACHINE_INFO,STG_SUB_2_MENU_POS,"4. Mac Info",stgMenu_MachineInfoCBS,1,0,0xFF,0,0,0,0},
	{STG_MENU_FACTORY_SETTINGS,STG_SUB_2_MENU_POS,"5. Default Setting",stgMenu_FactorySettingsCBS,1,0,0xFF,0,0,0,0},
};



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
	
	pModeMenu->action();

}



void Desk_Menu_ActionCBS(void)
{
	if(pModeMenu ->refreshScreenCmd == SCREEN_CMD_RESET)
	{
		pModeMenu ->refreshScreenCmd = SCREEN_CMD_NULL;

		hal_Oled_Clear();
		
		hal_Oled_ShowString(0,0,"w",8,1);	//8 *16
		hal_Oled_ShowString(16,20,"Away arm",24,1); //12*24
		showSystemTime();

		hal_Oled_Refresh();
	}

}

void Menu_Init(void)
{

	unsigned char i;
	//主设置菜单初始化,把菜单列表形成链表形式，方便调用
	settingModeMenu[1].pLase = &settingModeMenu[STG_MENU_SUM-1];
	settingModeMenu[1].pNext = &settingModeMenu[2];
	settingModeMenu[1].pParent = &settingModeMenu[STG_MENU_MAIN_SETTING];
	
	for(i=2; i<STG_MENU_SUM-1; i++)
	{
		settingModeMenu[i].pLase = &settingModeMenu[i-1];
		settingModeMenu[i].pNext = &settingModeMenu[i+1];
		settingModeMenu[i].pParent = &settingModeMenu[STG_MENU_MAIN_SETTING];
	}
	settingModeMenu[STG_MENU_SUM-1].pLase = &settingModeMenu[i-1];
	settingModeMenu[STG_MENU_SUM-1].pNext = &settingModeMenu[1];
	settingModeMenu[STG_MENU_SUM-1].pParent = &settingModeMenu[STG_MENU_MAIN_SETTING];
	

	pModeMenu = &generalModeMenu[GNL_MENU_DESKTOP];	//设置上电显示的菜单界面为桌面显示
	pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;	//更新刷新界面标志，进入界面后刷新全界面UI

}

static void gnlMenu_DesktopCBS(void)
{
	unsigned char keys;
	if(pModeMenu->refreshScreenCmd == SCREEN_CMD_RESET)
	{
		pModeMenu->refreshScreenCmd = SCREEN_CMD_NULL;
		pModeMenu->keyVal = 0xFF;
		 
		hal_Oled_Clear();
		
	 
		hal_Oled_ShowString(0,0,"N",8,1);
		
		hal_Oled_ShowString(16,20,"Away arm",24,1);
		showSystemTime();
		
		QueueEmpty(RFDRcvMsg);
	

		hal_Oled_Refresh();
		
	}
	if(pModeMenu->keyVal != 0xff)
	{
		keys = pModeMenu->keyVal;
		 
		pModeMenu->keyVal = 0xFF;	//恢复菜单按键值
		switch(keys)
		{
			case KEY6_LONG_PRESS:
				pModeMenu = &settingModeMenu[0];
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
		}
	}
}

//设置主菜单
static void stgMenu_MainMenuCBS(void)
{
	unsigned char keys;
	unsigned char i;
	unsigned char ClrScreenFlag;
	static stu_mode_menu *pMenu;		//用来保存当前选中的菜单
	static stu_mode_menu *bpMenu=0;		//用来备份上一次菜单选项，主要用于刷屏判断
	static unsigned char stgMainMenuSelectedPos=0;	//用来记录当前选中菜单的位置
	static stu_mode_menu *MHead,*MTail;		//这两个结构体指针是为了上下切换菜单时做翻页处理
	if(pModeMenu->refreshScreenCmd == SCREEN_CMD_RESET)
	{
		pModeMenu->refreshScreenCmd = SCREEN_CMD_NULL;
		pMenu = &settingModeMenu[0];
		hal_Oled_Clear();
		
		hal_Oled_ShowString(37,0,pMenu->pModeType,12,1);
		hal_Oled_Refresh();
	
		
		pMenu = &settingModeMenu[1];
		
		MHead = pMenu;			//记录当前显示菜单第一项
		MTail = pMenu+3;		//记录当前显示菜单最后一项,一页显示4行		
		bpMenu = 0;
 
		ClrScreenFlag = 1;
		stgMainMenuSelectedPos = 1;
		keys = 0xFF;
 
	}
	if(pModeMenu->keyVal != 0xff)
	{
		keys = pModeMenu->keyVal;
		pModeMenu->keyVal = 0xFF;	//恢复菜单按键值
		switch(keys)
		{
			case KEY1_CLICK:		//上
				
				if(stgMainMenuSelectedPos ==1)
				{
					MHead = MHead->pLase;
					pMenu = pMenu->pLase;
					MTail = MTail->pLase;
					stgMainMenuSelectedPos = 1;
					ClrScreenFlag = 1;
				}else
				{
					hal_Oled_ShowString(0,14*stgMainMenuSelectedPos,pMenu->pModeType,8,1);		//取消选中本菜单显示
					hal_Oled_Refresh();
					pMenu = pMenu->pLase;
					stgMainMenuSelectedPos--;
				}
			break;
			case KEY2_CLICK:		//下
				if(stgMainMenuSelectedPos ==4)
				{
					MHead = MHead->pNext;	
					pMenu = pMenu->pNext;
					MTail = pMenu;
					stgMainMenuSelectedPos = 4;
					ClrScreenFlag = 1;
				}else
				{
					hal_Oled_ShowString(0,14*stgMainMenuSelectedPos,pMenu->pModeType,8,1);		//取消选中本菜单显示
					hal_Oled_Refresh();
					pMenu = pMenu->pNext;																			//切换下一个选项
					stgMainMenuSelectedPos++;
				}

			break;
			
			case KEY5_CLICK_RELEASE:	//取消
				pModeMenu = &generalModeMenu[GNL_MENU_DESKTOP];;
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			
		}
	}
	if(bpMenu != pMenu)
	{
		bpMenu = pMenu;
		if(ClrScreenFlag)
		{
			
			ClrScreenFlag = 0;
			pMenu = MHead;
			hal_Oled_ClearArea(0,14,128,50);		//清屏
			hal_Oled_Refresh();
			for(i=1; i<5; i++)
			{
				hal_Oled_ShowString(0,14*i,pMenu->pModeType,8,1);
				hal_Oled_Refresh();
				pMenu = pMenu->pNext;
			} 
			pMenu = bpMenu;
			hal_Oled_ShowString(0,14*stgMainMenuSelectedPos,pMenu->pModeType,8,0);
			hal_Oled_Refresh();
			 
		}else
		{ 
			hal_Oled_ShowString(0,14*stgMainMenuSelectedPos,pMenu->pModeType,8,0);	
			hal_Oled_Refresh();
		}	
			
				 
	}
}

//探测器配对菜单处理函数
static void stgMenu_LearnSensorCBS(void)
{
}

//探测器列表菜单处理函数
static void stgMenu_DTCListCBS(void)
{
}

//wifi配网菜单处理函数
static void stgMenu_WifiCBS(void)
{
}

//设备信息菜单处理函数
static void stgMenu_MachineInfoCBS(void)
{
}

//恢复出厂设置菜单处理函数
static void stgMenu_FactorySettingsCBS(void)
{
}

//-----------------驱动层回调处理函数------------------------
 
//按键回调函数
static void KeyEventHandle(KEY_VALUE_TYPEDEF keys)
{
	pModeMenu->keyVal = keys;
	 
	// if((keys==KEY1_CLICK)
	// || (keys==KEY2_CLICK)
	// || (keys==KEY3_CLICK)
	// || (keys==KEY4_CLICK)
	// || (keys==KEY5_CLICK)
	// || (keys==KEY6_CLICK))
	// {
	// 	LedMsgInput(LED1,LED_LIGHT,1);
	// }else if((keys==KEY1_CLICK_RELEASE)
	// || (keys==KEY2_CLICK_RELEASE)
	// || (keys==KEY3_CLICK_RELEASE)
	// || (keys==KEY4_CLICK_RELEASE)
	// || (keys==KEY5_CLICK_RELEASE)
	// || (keys==KEY6_CLICK_RELEASE))
	// {
	// 	LedMsgInput(LED1,LED_BLINK4,1);
	// }else if((keys==KEY1_LONG_PRESS)
	// || (keys==KEY2_LONG_PRESS)
	// || (keys==KEY3_LONG_PRESS)
	// || (keys==KEY4_LONG_PRESS)
	// || (keys==KEY5_LONG_PRESS)
	// || (keys==KEY6_LONG_PRESS))
	// {
	// 	LedMsgInput(LED1,LED_DARK,1);
	// }
}


//RFD接收回调函数
static void RfdRcvHandle(unsigned char *pBuff)
{
	unsigned char temp;
	temp = '#';
	QueueDataIn(RFDRcvMsg, &temp, 1);
	QueueDataIn(RFDRcvMsg, &pBuff[0], 3);
	
	 
}








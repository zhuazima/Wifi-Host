
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
Queue8 RFDRcvMsg;	//RFD���ն���
LINK_STATE_TYPEDEF link_state;

stu_system_time stuSystemtime;		//ϵͳʱ��
stu_mode_menu *pModeMenu;		//ϵͳ��ǰ�˵�

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

//��ʼ������˵�
stu_mode_menu generalModeMenu[GNL_MENU_SUM] =
{
	{GNL_MENU_DESKTOP,DESKTOP_MENU_POS,"Desktop",gnlMenu_DesktopCBS,1,0,0xFF,0,0,0,0},
	
};	

//��ʼ�����ò˵�
stu_mode_menu settingModeMenu[STG_MENU_SUM] = 
{
	{STG_MENU_MAIN_SETTING,STG_MENU_POS,"Main Menu",stgMenu_MainMenuCBS,1,0,0xFF,0,0,0,0},		//������ҳ��
	{STG_MENU_LEARNING_SENSOR,STG_SUB_2_MENU_POS,"1. Learning Dtc",stgMenu_LearnSensorCBS,1,0,0xFF,0,0,0,0},	//̽����ѧϰ
	{STG_MENU_DTC_LIST,STG_SUB_2_MENU_POS,"2. Dtc List",stgMenu_DTCListCBS,1,0,0xFF,0,0,0,0},			//����


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
		case 1:	//����1
			hal_Oled_ShowString(106,54,"Mon",8,1);
		break;
		case 2:	//����2
			hal_Oled_ShowString(106,54,"Tue",8,1);
		break;
		case 3:	//����3
			hal_Oled_ShowString(106,54,"Wed",8,1);
		break;
		case 4:	//����4
			hal_Oled_ShowString(106,54,"Thu",8,1);
		break;
		case 5:	//����5
			hal_Oled_ShowString(106,54,"Fir",8,1);
		break;
		case 6:	//����6
			hal_Oled_ShowString(106,54,"Sat",8,1);
		break;
		case 7:	//����7
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
	//�����ò˵���ʼ��,�Ѳ˵��б��γ�������ʽ���������
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
	

	pModeMenu = &generalModeMenu[GNL_MENU_DESKTOP];	//�����ϵ���ʾ�Ĳ˵�����Ϊ������ʾ
	pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;	//����ˢ�½����־����������ˢ��ȫ����UI

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
		 
		pModeMenu->keyVal = 0xFF;	//�ָ��˵�����ֵ
		switch(keys)
		{
			case KEY6_LONG_PRESS:
				pModeMenu = &settingModeMenu[0];
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
		}
	}
}

//�������˵�
static void stgMenu_MainMenuCBS(void)
{
	unsigned char keys;
	unsigned char i;
	unsigned char ClrScreenFlag;
	static stu_mode_menu *pMenu;		//�������浱ǰѡ�еĲ˵�
	static stu_mode_menu *bpMenu=0;		//����������һ�β˵�ѡ���Ҫ����ˢ���ж�
	static unsigned char stgMainMenuSelectedPos=0;	//������¼��ǰѡ�в˵���λ��
	static stu_mode_menu *MHead,*MTail;		//�������ṹ��ָ����Ϊ�������л��˵�ʱ����ҳ����
	if(pModeMenu->refreshScreenCmd == SCREEN_CMD_RESET)
	{
		pModeMenu->refreshScreenCmd = SCREEN_CMD_NULL;
		pMenu = &settingModeMenu[0];
		hal_Oled_Clear();
		
		hal_Oled_ShowString(37,0,pMenu->pModeType,12,1);
		hal_Oled_Refresh();
	
		
		pMenu = &settingModeMenu[1];
		
		MHead = pMenu;			//��¼��ǰ��ʾ�˵���һ��
		MTail = pMenu+3;		//��¼��ǰ��ʾ�˵����һ��,һҳ��ʾ4��		
		bpMenu = 0;
 
		ClrScreenFlag = 1;
		stgMainMenuSelectedPos = 1;
		keys = 0xFF;
 
	}
	if(pModeMenu->keyVal != 0xff)
	{
		keys = pModeMenu->keyVal;
		pModeMenu->keyVal = 0xFF;	//�ָ��˵�����ֵ
		switch(keys)
		{
			case KEY1_CLICK:		//��
				
				if(stgMainMenuSelectedPos ==1)
				{
					MHead = MHead->pLase;
					pMenu = pMenu->pLase;
					MTail = MTail->pLase;
					stgMainMenuSelectedPos = 1;
					ClrScreenFlag = 1;
				}else
				{
					hal_Oled_ShowString(0,14*stgMainMenuSelectedPos,pMenu->pModeType,8,1);		//ȡ��ѡ�б��˵���ʾ
					hal_Oled_Refresh();
					pMenu = pMenu->pLase;
					stgMainMenuSelectedPos--;
				}
			break;
			case KEY2_CLICK:		//��
				if(stgMainMenuSelectedPos ==4)
				{
					MHead = MHead->pNext;	
					pMenu = pMenu->pNext;
					MTail = pMenu;
					stgMainMenuSelectedPos = 4;
					ClrScreenFlag = 1;
				}else
				{
					hal_Oled_ShowString(0,14*stgMainMenuSelectedPos,pMenu->pModeType,8,1);		//ȡ��ѡ�б��˵���ʾ
					hal_Oled_Refresh();
					pMenu = pMenu->pNext;																			//�л���һ��ѡ��
					stgMainMenuSelectedPos++;
				}

			break;
			
			case KEY5_CLICK_RELEASE:	//ȡ��
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
			hal_Oled_ClearArea(0,14,128,50);		//����
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

//̽������Բ˵�������
static void stgMenu_LearnSensorCBS(void)
{
}

//̽�����б�˵�������
static void stgMenu_DTCListCBS(void)
{
}

//wifi�����˵�������
static void stgMenu_WifiCBS(void)
{
}

//�豸��Ϣ�˵�������
static void stgMenu_MachineInfoCBS(void)
{
}

//�ָ��������ò˵�������
static void stgMenu_FactorySettingsCBS(void)
{
}

//-----------------������ص�������------------------------
 
//�����ص�����
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


//RFD���ջص�����
static void RfdRcvHandle(unsigned char *pBuff)
{
	unsigned char temp;
	temp = '#';
	QueueDataIn(RFDRcvMsg, &temp, 1);
	QueueDataIn(RFDRcvMsg, &pBuff[0], 3);
	
	 
}








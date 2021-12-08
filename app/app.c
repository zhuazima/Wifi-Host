
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
static void stgMenu_dl_EditCBS(void);
static void stgMenu_dl_DeleteCBS(void);
static void stgMenu_dl_ReviewCBS(void);
static void stgMenu_dl_ReviewMainCBS(void);

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

stu_mode_menu DL_ZX_Review[STG_MENU_DL_ZX_SUM] = 
{
	{STG_MENU_DL_ZX_REVIEW_MAIN,STG_SUB_2_MENU_POS,"View",stgMenu_dl_ReviewMainCBS,SCREEN_CMD_RESET,0,0xFF,0,0,0,0},	
	{STG_MENU_DL_ZX_REVIEW,STG_SUB_3_MENU_POS,"View",stgMenu_dl_ReviewCBS,SCREEN_CMD_RESET,0,0xFF,0,0,0,0},		 
	{STG_MENU_DL_ZX_EDIT,STG_SUB_3_MENU_POS,"Edit",stgMenu_dl_EditCBS,SCREEN_CMD_RESET,0,0xFF,0,0,0,0},
	{STG_MENU_DL_ZX_DELETE,STG_SUB_3_MENU_POS,"Delete",stgMenu_dl_DeleteCBS,SCREEN_CMD_RESET,0,0xFF,0,0,0,0},
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


			case KEY6_CLICK_RELEASE:	//确定
				pModeMenu->pChild = pMenu;
				pModeMenu = pModeMenu->pChild;
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			
		}
	}
	if(bpMenu != pMenu)		//选中菜单的记录
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
	unsigned char keys,dat,tBuff[3];
	static unsigned char PairingComplete = 0;
	static unsigned short Timer = 0;
	Stu_DTC stuTempDevice; 		//用于设置探测器参数时初始化探测器信息
	 
	if(pModeMenu->refreshScreenCmd == SCREEN_CMD_RESET)
	{
		pModeMenu->refreshScreenCmd = SCREEN_CMD_NULL;
		QueueEmpty(RFDRcvMsg);
		//pMenu = &settingModeMenu[0];
		hal_Oled_Clear();
		
		hal_Oled_ShowString(28,0,"Learning DTC",12,1);
		hal_Oled_ShowString(43,28,"Pairing...",8,1);
		
		hal_Oled_Refresh();
		
		keys = 0xFF;
		PairingComplete = 0; 
		Timer = 0;
		 
	}
	
	if(QueueDataLen(RFDRcvMsg) && (!PairingComplete))
	{
		QueueDataOut(RFDRcvMsg,&dat);
		if(dat == '#')
		{
			QueueDataOut(RFDRcvMsg,&tBuff[2]);
			QueueDataOut(RFDRcvMsg,&tBuff[1]);
			QueueDataOut(RFDRcvMsg,&tBuff[0]);
			hal_Oled_ClearArea(0,28,128,36);		//清屏
			
			
			stuTempDevice.Code[2] = tBuff[2];
			stuTempDevice.Code[1] = tBuff[1];
			stuTempDevice.Code[0] = tBuff[0];
			
			if((stuTempDevice.Code[0]==SENSOR_CODE_DOOR_OPEN) ||
			(stuTempDevice.Code[0]==SENSOR_CODE_DOOR_CLOSE) ||
			(stuTempDevice.Code[0]==SENSOR_CODE_DOOR_TAMPER)||
			(stuTempDevice.Code[0]==SENSOR_CODE_DOOR_LOWPWR))
			{
				//是无线门磁码
				stuTempDevice.DTCType = DTC_DOOR;
				
			}else if((stuTempDevice.Code[0]==SENSOR_CODE_REMOTE_ENARM) ||
			(stuTempDevice.Code[0]==SENSOR_CODE_REMOTE_DISARM) ||
			(stuTempDevice.Code[0]==SENSOR_CODE_REMOTE_HOMEARM) ||
			(stuTempDevice.Code[0]==SENSOR_CODE_REMOTE_SOS))
			{
				//无线遥控器码
				stuTempDevice.DTCType = DTC_REMOTE;
			}else if((stuTempDevice.Code[0]==SENSOR_CODE_PIR)
			|| (stuTempDevice.Code[0]==SENSOR_CODE_PIR_LOWPWR)
			|| (stuTempDevice.Code[0]==SENSOR_CODE_PIR_TAMPER))
			{
				//无线红外码
				stuTempDevice.DTCType = DTC_PIR_MOTION;
			}
			stuTempDevice.ZoneType = ZONE_TYP_1ST;
			if(AddDtc(&stuTempDevice) != 0xFF)
			{
				switch(stuTempDevice.DTCType)
				{
					case DTC_DOOR:
						hal_Oled_ShowString(34,28,"Success!",8,1);
						hal_Oled_ShowString(16,36,"Added door dtc..",8,1);
					break;
					case DTC_REMOTE:
						hal_Oled_ShowString(34,28,"Success!",8,1);
						hal_Oled_ShowString(7,36,"Added remote dtc..",8,1);
					break;
					case DTC_PIR_MOTION:
						hal_Oled_ShowString(34,28,"Success!",8,1);
						hal_Oled_ShowString(19,36,"Added pir dtc..",8,1);
					break;
				}
				
				hal_Oled_Refresh();
				PairingComplete = 1;		//配对完成标志
				Timer = 0;		 
			}else
			{
				hal_Oled_ShowString(34,28,"Fail...",8,1);
				hal_Oled_Refresh();
			}
			
		}
		
		
	}
	
	if(pModeMenu->keyVal != 0xff)
	{
		keys = pModeMenu->keyVal;
		pModeMenu->keyVal = 0xFF;	//恢复菜单按键值
		switch(keys)
		{
			case KEY5_CLICK_RELEASE:	//取消
				pModeMenu = pModeMenu->pParent;
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY5_LONG_PRESS:		//返回桌面
				pModeMenu = &generalModeMenu[GNL_MENU_DESKTOP];;;
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
		}
	}

	if(PairingComplete)
	{
		Timer++;
		if(Timer > 150)//+1=10ms,10*150=1500ms=1.5s
		{
			Timer = 0;
			pModeMenu = pModeMenu->pParent;			//1.5秒时间到，自动返回父级菜单
			pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
		}
	}
}


//探测器列表菜单处理函数
static void stgMenu_DTCListCBS(void)
{
	unsigned char keys;
	unsigned char i,j;
	unsigned char ClrScreenFlag;
	Stu_DTC tStuDtc;
	 
	//static Stu_DTC StuDTCtemp[PARA_DTC_SUM];
	static unsigned char DtcNameBuff[PARA_DTC_SUM][16];
	static stu_mode_menu settingMode_DTCList_Sub_Menu[PARA_DTC_SUM];
	static stu_mode_menu *pMenu;
	static stu_mode_menu *bpMenu=0;		//用来备份上一次菜单选项，主要用于刷屏判断
	static unsigned char stgMainMenuSelectedPos=0;	//用来记录当前选中菜单的位置
	static stu_mode_menu *MHead,*MTail;		//这两个结构是为了上下切换菜单时做翻页处理
	static unsigned char pMenuIdx=0;		//用来动态指示菜单下标,最终这个就是已学习探测器的总数量	
	if(pModeMenu->refreshScreenCmd == SCREEN_CMD_RESET)
	{
		pModeMenu->refreshScreenCmd = SCREEN_CMD_NULL;
		
		pMenuIdx = 0;
		stgMainMenuSelectedPos = 1;
		bpMenu = 0;
		ClrScreenFlag = 1;
		 
		keys = 0xFF;
		
		pMenu = settingMode_DTCList_Sub_Menu;
		
		hal_Oled_Clear();
		hal_Oled_ShowString(40,0,"Dtc List",12,1);
		hal_Oled_Refresh();
		
		//逐个判断，把配对的探测器都找出来
		for(i=0; i<PARA_DTC_SUM; i++)
		{
			if(CheckPresenceofDtc(i))
			{
				GetDtcStu(&tStuDtc,i);
				(pMenu+pMenuIdx)->ID = pMenuIdx;
				//(pMenu+0)->ID = 0;
				
				(pMenu+pMenuIdx)->menuPos = STG_SUB_3_MENU_POS;
				(pMenu+pMenuIdx)->reserved = tStuDtc.ID-1;
				//StuDTCtemp[pMenuIdx].ID = tStuDtc.ID;
				for(j=0; j<16; j++)
				{
				//	StuDTCtemp[pMenuIdx].Name[j] = tStuDtc.Name[j];
					DtcNameBuff[pMenuIdx][j] = tStuDtc.Name[j];
				}
			 
				//(pMenu+pMenuIdx)->pModeType = StuDTCtemp[pMenuIdx].Name;
				
				(pMenu+pMenuIdx)->pModeType = DtcNameBuff[pMenuIdx];
				pMenuIdx++;
			}
		}
		
		if(pMenuIdx != 0)
		{
			//有探测器存在的情况
			if(pMenuIdx > 1)
			{
				pMenu->pLase =  pMenu+(pMenuIdx-1);
				pMenu->pNext =  pMenu+1;
				pMenu->pParent = &settingModeMenu[STG_MENU_MAIN_SETTING];
				for(i=1; i<pMenuIdx-1; i++)
				{
					(pMenu+i)->pLase =  pMenu+(i-1);
					(pMenu+i)->pNext = pMenu+(i+1);
					(pMenu+i)->pParent = &settingModeMenu[STG_MENU_MAIN_SETTING];
				}
				(pMenu+(pMenuIdx-1))->pLase =  pMenu+(i-1);
				(pMenu+(pMenuIdx-1))->pNext = pMenu;
				(pMenu+(pMenuIdx-1))->pParent = &settingModeMenu[STG_MENU_MAIN_SETTING];
			}else if(pMenuIdx == 1)
			{

				pMenu->pLase = pMenu;
				pMenu->pNext = pMenu;
				pMenu->pParent = &settingModeMenu[STG_MENU_MAIN_SETTING];
			}
		}else
		{
			//没有探测器
			bpMenu = pMenu;
			hal_Oled_ShowString(0,14," No detectors.",8,1);
			hal_Oled_Refresh();
		}

		MHead = pMenu;			//记录当前显示菜单第一项
		if(pMenuIdx < 2)
		{
			MTail = pMenu;
		}else if(pMenuIdx < 5)
		{
			MTail = pMenu+(pMenuIdx-1);
		}else
		{
			MTail = pMenu+3;		//记录当前显示菜单最后一项,一页显示4行
		}
		
	}else if(pModeMenu->refreshScreenCmd==SCREEN_CMD_RECOVER)
	{	
		pModeMenu->refreshScreenCmd = SCREEN_CMD_NULL;
		//恢复之前的选择位置显示
		hal_Oled_Clear();
		hal_Oled_ShowString(40,0,"Dtc List",12,1);
		hal_Oled_Refresh();
		keys = 0xFF;
		ClrScreenFlag = 1;
		bpMenu = 0;
		
	}
	
	if(pModeMenu->keyVal != 0xff)
	{
		keys = pModeMenu->keyVal;
		pModeMenu->keyVal = 0xFF;	//恢复菜单按键值
		switch(keys)
		{
			case KEY1_CLICK_RELEASE:		//上
				if(pMenuIdx < 2)
				{
					//只有一个探测器不做处理
				}else if(pMenuIdx < 5)
				{
					//只有一页，也就是只有4个探测器的时候
					if(stgMainMenuSelectedPos ==1)	//判断是否选中的是第一行
					{
						//头尾指针不变，只把当前菜单指向上一个
						stgMainMenuSelectedPos = pMenuIdx;
						ClrScreenFlag = 1;
						pMenu = pMenu->pLase;
						
					}else 
					{
						//不清屏，直接刷新局部显示
						hal_Oled_ShowString(0,14*stgMainMenuSelectedPos,pMenu->pModeType,8,1);		//取消选中本菜单显示
						hal_Oled_Refresh();
						pMenu = pMenu->pLase;
						stgMainMenuSelectedPos--;
					}
				}else if(pMenuIdx > 4)	//当前探测器超过4个
				{
					if(stgMainMenuSelectedPos ==1)	//判断是否选中的是第一行
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
				}
			break;
			
			case KEY2_CLICK_RELEASE:		//下 
				if(pMenuIdx < 2)
				{
					//只有一个探测器不做处理
				}else if(pMenuIdx < 5)
				{
					//只有一页，也就是只有4个探测器的时候
					if(stgMainMenuSelectedPos ==pMenuIdx)	//判断是否选中的是第4行
					{
						//头尾指针不变，只把当前菜单指向下个
						pMenu = pMenu->pNext;
						stgMainMenuSelectedPos = 1;
						ClrScreenFlag = 1;
						
					}else 
					{
						//不清屏，直接刷新局部显示
						hal_Oled_ShowString(0,14*stgMainMenuSelectedPos,pMenu->pModeType,8,1);		//取消选中本菜单显示
						hal_Oled_Refresh();
						pMenu = pMenu->pNext;																			//切换下一个选项
						stgMainMenuSelectedPos++;
					}
				}else if(pMenuIdx > 4)	//当前探测器超过4个
				{
					if(stgMainMenuSelectedPos ==4)	//判断是否选中的是第一行
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
				}
			break;
			
			case KEY6_CLICK_RELEASE:			//确定
				if(pMenuIdx>0)
				{
					pModeMenu = &DL_ZX_Review[STG_MENU_DL_ZX_REVIEW_MAIN]; 
					pModeMenu->reserved = pMenu->reserved;	//这里用于传递后面要查看、修改、删除探测器的ID号
					pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
				}
			break;
			
			case KEY5_CLICK_RELEASE:	//取消
				pModeMenu = pModeMenu->pParent;
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RECOVER;
			break;
			case KEY5_LONG_PRESS:		//返回桌面
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
			if(pMenuIdx <4)
			{
				for(i=0; i<pMenuIdx; i++)
				{
					hal_Oled_ShowString(0,14*(i+1),pMenu->pModeType,8,1);
					hal_Oled_Refresh();
					pMenu = pMenu->pNext;
				}
			}else
			{
				for(i=1; i<5; i++)
				{
					hal_Oled_ShowString(0,14*i,pMenu->pModeType,8,1);
					hal_Oled_Refresh();
					pMenu = pMenu->pNext;
				} 
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

static void stgMenu_dl_ReviewMainCBS(void)
{
	
}

static void stgMenu_dl_ReviewCBS(void)
{

}
static void stgMenu_dl_EditCBS(void)
{

}
static void stgMenu_dl_DeleteCBS(void)
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








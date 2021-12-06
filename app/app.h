#ifndef _APP_H
#define _APP_H

typedef enum 
{
    LINKSTATE_WAIT,
    LINKSTATE_LINKING,
    LINKSTATE_CONNECTED
}LINK_STATE_TYPEDEF;

//----菜单相关声明区域
typedef enum
{
	SCREEN_CMD_NULL,		//无用命令
	SCREEN_CMD_RESET,		//重置屏显示
	SCREEN_CMD_RECOVER,		//恢复原来显示
	SCREEN_CMD_UPDATE,		//更新原来显示
}SCREEN_CMD;		//刷新屏显示标志

//普通菜单列表
typedef enum
{

	GNL_MENU_DESKTOP,		//桌面
	GNL_MENU_SUM,
}GENERAL_MENU_LIST;			//普通菜单列表

//设置菜单列表ID
typedef enum
{
	STG_MENU_MAIN_SETTING,
	STG_MENU_LEARNING_SENSOR,
	STG_MENU_DTC_LIST,
	STG_MENU_WIFI,
	//STG_MENU_TIME,
	STG_MENU_MACHINE_INFO,
	STG_MENU_FACTORY_SETTINGS,
	STG_MENU_SUM
}STG_MENU_LIST;

//定义菜单的位置，主要用于超时退出判断
typedef enum
{
	DESKTOP_MENU_POS,	//桌面
	STG_MENU_POS,
	STG_WIFI_MENU_POS,
	STG_SUB_2_MENU_POS,
	STG_SUB_3_MENU_POS,
	STG_SUB_4_MENU_POS,
}MENU_POS;

typedef struct MODE_MENU
{
	unsigned char ID;				//菜单唯一ID号
	MENU_POS menuPos;				//当前菜单的位置信息
	unsigned char *pModeType;		//指向当前模式类型
	void (*action)(void);				//当前模式下的响应函数
	SCREEN_CMD refreshScreenCmd;		//刷新屏显示命令
	unsigned char reserved;				//预留，方便参数传递
	unsigned char keyVal;				//按键值,0xFF代表无按键触发
	struct MODE_MENU *pLase;			//指向上一个选项
	struct MODE_MENU *pNext;			//指向下一个选项
	struct MODE_MENU *pParent;			//指向父级菜单
	struct MODE_MENU *pChild;			//指向子级菜单
}stu_mode_menu;

typedef enum
{
	STG_MENU_DL_ZX_REVIEW_MAIN,
	STG_MENU_DL_ZX_REVIEW,
	STG_MENU_DL_ZX_EDIT,
	STG_MENU_DL_ZX_DELETE,
	STG_MENU_DL_ZX_SUM,
}STG_MENU_DZ_ZX_LIST;



typedef struct SYSTEM_TIME
{
	unsigned short year;
	unsigned char mon;
	unsigned char day;
	unsigned char week;
	unsigned char hour;
	unsigned char min;
	unsigned char sec;
}stu_system_time;


void AppInit(void);
void AppProc(void);

#endif


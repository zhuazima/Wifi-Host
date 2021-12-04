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


typedef struct MENU_STRUCT
{
    unsigned char ID;
    unsigned char *Name;
    SCREEN_CMD refreshScreenCmd;		//刷新屏显示命令
    void (*action)(void);
    struct MENU_STRUCT *Last_Menu;
    struct MENU_STRUCT *Next_Menu;
    struct MENU_STRUCT *Father_Menu;
    struct MENU_STRUCT *ChiLdren_Menu;

}stu_MENU_TYPEDEF;

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


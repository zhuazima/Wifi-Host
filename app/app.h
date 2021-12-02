#ifndef _APP_H
#define _APP_H

typedef enum 
{
    LINKSTATE_WAIT,
    LINKSTATE_LINKING,
    LINKSTATE_CONNECTED
}LINK_STATE_TYPEDEF;


typedef struct MENU_STRUCT
{
    unsigned char ID;
    unsigned char *Name;
    void (*action)(void);
    struct MENU_STRUCT *Last_Menu;
    struct MENU_STRUCT *Next_Menu;
    struct MENU_STRUCT *Father_Menu;
    struct MENU_STRUCT *ChiLdren_Menu;

}stu_MENU_TYPEDEF;

typedef struct
{
    unsigned short year;
    unsigned char month;
    unsigned char day;
    unsigned char weekday;
    unsigned char hour;
    unsigned char min;
    unsigned char sec;
}stu_DATA_TYPEDEF;



void AppInit(void);
void AppProc(void);

#endif


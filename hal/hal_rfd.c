#include "hal_rfd.h"
#include "stm32F10x.h"
#include "hal_time.h"
#include "OS_System.h"


Queue32 RFD_RxBuff;


static void hal_Rfd_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_InitStructure.GPIO_Pin = RFD_GPIO_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
	GPIO_Init(RFD_GPIO_PORT, &GPIO_InitStructure);



    hal_CreatTimer(T_RFD_PULSH_RCV,hal_PulseAQTHandler,1,T_STA_START);              //50us
    hal_CreatTimer(T_RFD_PULSH_RCV,hal_RFTDecodeFilterHandler,20000,T_STA_START);   //1s
}


static unsigned char Get_RFDIO_Status(void)
{
    return (GPIO_ReadInputDataBit(RFD_GPIO_PORT,RFD_GPIO_PIN));
}


void hal_Rfd_Init(void)
{
    hal_Rfd_Config();


}

void hal_RfdProc(void)
{

}

//50us 定时器处理函数
void hal_PulseAQTHandler(void)
{
    unsigned char sta;
    static unsigned char temp,i;
    sta = Get_RFDIO_Status();
    if(sta)
    {
        temp |= (1 << i++);
    }
    else
    {
        temp &= (0 << i++);
    }
    if (i == 8)
    {
        i = 0;
        QueueDataIn(RFD_RxBuff,temp,1);
    }
}


void hal_RFTDecodeFilterHandler(void)
{

}
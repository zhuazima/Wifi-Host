/*
 * @Author: your name
 * @Date: 2021-11-20 19:28:49
 * @LastEditTime: 2021-11-22 19:01:21
 * @LastEditors: Please set LastEditors
 * @Description: 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 * @FilePath: \Wifi Host\hal\hal_led.h
 */
#ifndef _HAL_LED_H
#define _HAL_LED_H




#define LED1_PORT			GPIOA
#define LED1_PIN			GPIO_Pin_1

#define BUZ_PORT			GPIOB
#define BUZ_PIN			GPIO_Pin_0

#define LED_EFFECT_END	0xFFFE
#define LED_EFFECT_AGN	0xFFFF


typedef enum
{
	LED1,
	BUZ,
	LED_SUM
}LED_TYPEDEF;


typedef enum
{
	LED_DARK,
	LED_LIGHT,
	LED_LIGHT_100MS,
	LED_BLINK1,
	LED_BLINK2,
	LED_BLINK3,
	LED_BLINK4,
	 
}LED_EFFECT_TEPEDEF;


void hal_Led1Drive(unsigned char sta);
void hal_BuzDrive(unsigned char sta);

void hal_LedTurn(void);

void hal_LedInit(void);
void hal_LedProc(void);

void LedMsgInput(unsigned char type,LED_EFFECT_TEPEDEF cmd,unsigned char clr);




#endif


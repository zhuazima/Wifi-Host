#ifndef _HAL_LED_H
#define _HAL_LED_H

#define LED1_PORT			GPIOA
#define LED1_PIN			GPIO_Pin_1

#define BUZ_PORT			GPIOB
#define BUZ_PIN			GPIO_Pin_0



void hal_Led1Drive(unsigned char sta);
void hal_BuzDrive(unsigned char sta);

void hal_LedTurn(void);

void hal_LedInit(void);
void hal_LedProc(void);
#endif

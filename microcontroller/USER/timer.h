#ifndef __TIMER_H
#define __TIMER_H

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"

#define SYSCLK 72000000
#define PRESCALER 72

void GPIO_Configuration(void);
void TIM_Configuration(TIM_TypeDef* TIMx);
void TIM_Init(void);
    
#endif

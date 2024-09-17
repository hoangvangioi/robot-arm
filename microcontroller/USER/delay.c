#include "delay.h"

static uint16_t D_us=0;
static uint16_t D_ms=0;

void Delay_Init(void)
{
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
	
	D_us = SystemCoreClock/8000000;
	D_ms = (uint16_t)D_us * 1000;
	
}

void Delay_us(uint32_t nus)
{
	uint32_t temp;
	SysTick->CTRL = 0x00;
	SysTick->LOAD = nus*D_us;
	SysTick->VAL  = 0x00;
	SysTick->CTRL|= 0x01 ;
	do {
		temp=SysTick->CTRL;
	} while((temp&0x01)&&!(temp&(1<<16)));
	SysTick->CTRL = 0x00;
	SysTick->VAL  = 0X00;
}

void Delay_ms(uint32_t nms)
{
	uint32_t temp;
	SysTick->CTRL = 0x00;
	SysTick->LOAD = nms*D_ms;
	SysTick->VAL  = 0x00;
	SysTick->CTRL|= 0x01 ;
	do {
		temp=SysTick->CTRL;
	} while((temp&0x01)&&!(temp&(1<<16)));
	SysTick->CTRL = 0x00;
	SysTick->VAL  = 0X00;
}

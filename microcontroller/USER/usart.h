#ifndef __USART_H
#define __USART_H

#include "stdio.h"
#include "stdint.h"
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_usart.h"

#define USAR_LEN					1024
#define USART1_LEN  			USAR_LEN

extern uint16_t USART1_RecLEN;
extern uint8_t USART1_RX_BUF[USART1_LEN];

void USART1_Init(uint32_t baud_rate);
void USART_SendByte(USART_TypeDef* USARTx, uint8_t SendByte);
void USART_SendString(USART_TypeDef* USARTx, const char* SendString);
void USART_SendChars(USART_TypeDef* USARTx, const uint8_t* SendChars,uint16_t len);

#endif

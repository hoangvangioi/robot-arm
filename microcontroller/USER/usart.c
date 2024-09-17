#include "usart.h"

uint8_t USART1_RX_BUF[USART1_LEN];
uint16_t USART1_RecLEN;

void USART1_Init(uint32_t baud_rate){
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
    
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	USART_InitStructure.USART_BaudRate = baud_rate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init(USART1, &USART_InitStructure);
	USART_Cmd(USART1, ENABLE);
}

void USART_SendByte(USART_TypeDef* USARTx, uint8_t SendByte)
{
    USARTx->DR = (SendByte & (uint16_t)0x01FF);
    /* Loop until the end of transmission */
    while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
}

void USART_SendString(USART_TypeDef* USARTx, const char* str)
{
	while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
	while(* str)
	{
    	USARTx->DR = (*str& (uint16_t)0x01FF);
  		/* Loop until the end of transmission */
		str ++;
		while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
	}
}

void USART_SendChars(USART_TypeDef* USARTx, const uint8_t* SendChars,uint16_t len)
{
	uint16_t i = 0;
	while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
	for(i = 0; i < len; i++)
	{
    	USARTx->DR = (*(SendChars + i) & (uint16_t)0x01FF);
		while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
	}
}

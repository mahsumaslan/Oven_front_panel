/*
 * uart.c
 *
 *  Created on: Mar 4, 2017
 *      Author: mahsum aslan
 */

#include "uart.h"
#include "rcc.h"
#include "gpio.h"

// uart settings
#define UART3_TX	 (0x1U<<8)
#define UART3_RX	 (0x1U<<9)
#define USART3_EN	 (0x1U<<18)
// 8bit word lengt; Start bit + 8 data bits + n stop bit
#define UART_DATAWITH_8B	(0x00000000U)
// parity control
#define UART_PARITY_NONE	(0x00000000U)
// uart stop bit 1
#define UART_STOPBITS_1		(0x00000000U)

static uint16_t compute_uart_div(uint32_t PeriphClk, uint32_t Baudrate){
	return ((PeriphClk + (Baudrate/2U)) / Baudrate);
}

static void uart_set_baudrate(USART_TypeDef * USARTx, uint32_t PeriphClk, uint32_t Baudrate){
	USARTx->BRR = compute_uart_div(PeriphClk, Baudrate);
}

static void set_uart_transfer_direction(USART_TypeDef * USARTX, uint32_t transferDirection){
	MODIFY_REG( USARTX->CR1, (USART_CR1_RE | USART_CR1_TE), transferDirection );
}

static void config_uart_parameters(USART_TypeDef *USARTx, uint32_t DataWith, uint32_t Parity, uint32_t StopBits){
	MODIFY_REG( USARTx->CR1, (USART_CR1_PS | USART_CR1_PCE | USART_CR1_M), (Parity | DataWith) );
	MODIFY_REG( USARTx->CR2, USART_CR2_STOP, StopBits );
}

static void uart_enable(USART_TypeDef* USARTx){
	SET_BIT(USARTx->CR1, USART_CR1_UE);
}

void uart_write(USART_TypeDef *USARTx, uint8_t data){
	// check data reg is empty
	while (!(USARTx->ISR & USART_ISR_TXE));
	// write data into data reg
	USARTx->TDR = data;
}

int __io_putchar(int ch){
	// retargeting printf
	uart_write(USART3, ch);
	return ch;
}

uint8_t uart_read(USART_TypeDef *USARTx){
	//while( !(USARTx->ISR & USART_ISR_RXNE) ){}
	if (USARTx->ISR & USART_ISR_RXNE)
		return ( READ_BIT(USARTx->RDR, USART_RDR_RDR) & 0xFFU );
	else
		return -1;
}

void uart3_init(void){
	// PD8 = TX
	// PD9 = RX
	/*********** configure tx pin ***********/
	// enable GPIOD
	set_ahb1_periph_clock(GPIOD_CLK_EN);
	// set PD8 to alternate function
	set_pin_mode(GPIOD, UART3_TX, GPIO_ALTERNATE_MODE);
	// set alternate func type to USART
	// AFRL = AFR[0], AFRH = AFR[1]
	GPIOD->AFR[1] |= (0x1U << 0);
	GPIOD->AFR[1] |= (0x1U << 1);
	GPIOD->AFR[1] |= (0x1U << 2);
	GPIOD->AFR[1] &= ~(0x1U << 3);

	/*********** configure rx pin ***********/
	// set PD9 to alternate function
	set_pin_mode(GPIOD, UART3_RX, GPIO_ALTERNATE_MODE);
	// set alternate func type to USART
	// AFRL = AFR[0], AFRH = AFR[1]
	GPIOD->AFR[1] |= (0x1U << 4);
	GPIOD->AFR[1] |= (0x1U << 5);
	GPIOD->AFR[1] |= (0x1U << 6);
	GPIOD->AFR[1] &= ~(0x1U << 7);

	// enable clock to the USART3 module
	set_apb1_periph_clock(USART3_EN);

	// configure USART parameters
	config_uart_parameters(USART3, UART_DATAWITH_8B, UART_PARITY_NONE, UART_STOPBITS_1);
	set_uart_transfer_direction(USART3, USART_CR1_TE | USART_CR1_RE);

	// set baudrate
	uart_set_baudrate( USART3, 16000000, 115200);

	// enable USART
	uart_enable(USART3);
}

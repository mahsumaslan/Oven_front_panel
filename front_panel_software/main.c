/*
 * main.c
 *
 *  Created on: Mar 4, 2017
 *      Author: mahsum aslan
 */

#include <stdio.h>
#include <stdbool.h>
#include "stm32f7xx.h"
#include "gpio.h"
#include "uart.h"
#include "sseg.h"
#include "rcc.h"

int main(void) {

	uart3_init();

	// enable clock for port A
	set_ahb1_periph_clock(GPIOA_CLK_EN);
	// enable clock for port F
	set_ahb1_periph_clock(GPIOF_CLK_EN);

	// set PA3, PA5 and PA6 pins as output
	set_pin_mode(GPIOA, data_pin, GPIO_OUTPUT_MODE);
	set_pin_mode(GPIOA, store_pin, GPIO_OUTPUT_MODE);
	set_pin_mode(GPIOA, clock_pin, GPIO_OUTPUT_MODE);

	// set PF13 pin as input
	set_pin_mode(GPIOF, (0x1U<<13), GPIO_INPUT_MODE);

	init_seven_segment();

	// button and related variables
	char ch = 0;
	int num = 0;
	int prev_num = -1;
	int button = 0;
	int prev_button = 0;

	// check which buttons pressed
	while (true) {
		// let it count continuously
		//for (int k = 0; k < 60000; ++k);
		//++num;

		// check the interval
		if (num > 9999) num = 0;
		else if (num < 0) num = 9999;

		// send data to console
		if(!(num == prev_num))
			printf("%d\n",num);

		// write already processed number
		write_number(num);

		// remember previous number
		// if changes than we know we should send
		prev_num = num;

		// read the console
		ch = uart_read(USART3);
		switch (ch){
			case '0':
				num = 0;	break;
			case '1':
				++num;		break;
			case '2':
				--num;		break;
			case '3':
				num -= 10;	break;
			case '4':
				num += 10;	break;
			case '5':
				num += 100;	break;
			case '6':
				num -= 100;	break;
			default:
				// not pressed
		}

		// button operations
		button = button_check();
		if (button == prev_button) continue;

		switch (button){
			case 0:
				num = 0;	break;
			case 1:
				++num;		break;
			case 2:
				--num;		break;
			case 3:
				num -= 10;	break;
			case 4:
				num += 10;	break;
			case 5:
				num += 100;	break;
			case 6:
				num -= 100;	break;
			default:
				// not pressed
		}
		prev_button = button;
	}
}

/*
 * sseg.c
 *
 *  Created on: Mar 4, 2017
 *      Author: mahsum aslan
 */

#include "sseg.h"

// 2A 2F 2E 2D 2C 2B 2G 2H 1A 1B 1C 1D 1E 1F 1G 1H
// -------segments--------				  --digits--
// a, b, c, d,  e, f, g, x                4, 3, 2, 1
// 1, 1, 1, 1,  1, 1, 1, 1,  0, 0, 0, 0,  0, 0, 0, 1

//	global numbers   0	     1	   	 2       3       4       5       6       7	   	 8       9
static int numbers[10] = {0xFC00, 0x6000, 0xDA00, 0xF200, 0x6600, 0xB600, 0xBE00, 0xE000, 0xFE00, 0xF600};

static void data_send(int data) {
	data ^= 0xFF00;
	for (int i = 0; i < 16; ++i) {
		if (data & (1 << i)) {
			GPIOA->ODR ^= data_pin;
			GPIOA->ODR ^= clock_pin;
			GPIOA->ODR ^= clock_pin;
			GPIOA->ODR ^= data_pin;
		} else {
			GPIOA->ODR ^= clock_pin;
			GPIOA->ODR ^= clock_pin;
		}
	}
	GPIOA->ODR ^= store_pin;
	GPIOA->ODR ^= store_pin;
}

int button_check(void) {
	for (int i = 0; i < 7; ++i) {
		data_send((0x8000 >> i));
		if ( !READ_BIT(GPIOF->IDR, btn) ) return (i);
	}
	return 9;
}

void write_number(int num){
	for (int i = 0; i < 4; ++i) {
		data_send(numbers[(num%10)] | (1 << i) );
		num /= 10;
	}
}

void init_seven_segment(void){
	// load			  -		 -		-	   -
	int data[4] = { 0x201, 0x202, 0x204, 0x208 };
	for (int j = 1; j < 10; (j+=2)){
		for (int i = 0; i < 4; ++i) {
			data_send(data[i]);
			for (int k = 0; k < 85000; ++k);
		}
	}
	// write load		L		o		a		d
	int datas[4] = { 0x1C08, 0x3A04, 0xEE02, 0x7A01 };
	for (int k = 0; k < 3000; ++k){
		for (int i = 0; i < 4; ++i) {
			data_send(datas[i]);
		}
	}
}

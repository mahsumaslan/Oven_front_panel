/*
 * gpio.c
 *
 *  Created on: Mar 4, 2017
 *      Author: mahsum aslan
 */

#include "gpio.h"

void set_pin_mode(GPIO_TypeDef *GPIOx, uint32_t Pin, uint32_t Mode){
   /* clear the relevant bits in the mode register
	* set the relevant bits int the mode register
    * first select mode of given port and send position val func.
	* later reverse the value and count leading zero. multiply
	* counted number by two. lastly shift two one bit (0x3)
	* by counted zero. finally it gets pos val and multiply by
	* two and shifts mode by that number */
	MODIFY_REG(GPIOx->MODER, (0x3 << (POSITION_VAL(Pin) * 2U)), (Mode << (POSITION_VAL(Pin) * 2U)));
}

#ifndef MICROBLAZE_BUTTON_HPP_
#define MICROBLAZE_BUTTON_HPP_

#ifdef __MICROBLAZE__

#include "xparameters.h"

extern const volatile unsigned* BUTTON_ADDRESS;

typedef enum {
	BTN_NONE 	= 0b00000,
	BTNU 		= 0b00001,
	BTNL 		= 0b00010,
	BTNR 		= 0b00100,
	BTND 		= 0b01000,
	BTNC 		= 0b10000,
} Button;

uint16_t GetButtonValue();


#endif  // __MICROBLAZE__
#endif  // MICROBLAZE_BUTTON_HPP_

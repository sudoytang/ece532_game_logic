#ifndef MICROBLAZE_BUTTON_HPP_
#define MICROBLAZE_BUTTON_HPP_

#ifdef __MICROBLAZE__

#include "xparameters.h"
#include <cstdint>
extern const volatile unsigned* BUTTON_ADDRESS;
extern const volatile unsigned* SWITCH_ADDRESS;
typedef enum {
	BTN_NONE 	= 0b00000,
	BTNC 		= 0b00001,
	BTNU 		= 0b00010,
	BTNL 		= 0b00100,
	BTNR 		= 0b01000,
	BTND 		= 0b10000,

} Button;

uint16_t GetButtonValue();

typedef enum {
	SW_GYRO0_RESET	= 1 << 0,
	SW_GYRO0_CALIB  = 1 << 1,
	SW_GYRO1_RESET  = 1 << 15,
	SW_GYRO1_CALIB  = 1 << 14,
	SW_BACK_TO_MAIN	= 1 << 7,
	SW_SOFT_RESET	= 1 << 8,
} Switch;

uint16_t GetSwitchValue();


#endif  // __MICROBLAZE__
#endif  // MICROBLAZE_BUTTON_HPP_

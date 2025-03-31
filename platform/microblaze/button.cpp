#include <cstdint>
#include "button.hpp"

const volatile unsigned* BUTTON_ADDRESS = (unsigned*)(XPAR_GYRO0_RAW_GPIO_BASEADDR + 0x8);

uint16_t GetButtonValue() {
	unsigned res = *BUTTON_ADDRESS;
	return (short)((res >> 16) & 0xffff);
}

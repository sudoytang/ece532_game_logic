#include "button_switch.hpp"

#include <cstdint>

const volatile unsigned* BUTTON_ADDRESS = (unsigned*)(XPAR_AXI_GPIO_CTRL_BASEADDR + 0x0);
const volatile unsigned* SWITCH_ADDRESS = (unsigned*)(XPAR_AXI_GPIO_CTRL_BASEADDR + 0x8);
uint16_t GetButtonValue() {
	unsigned res = *BUTTON_ADDRESS;
	return uint16_t(res);
}

uint16_t GetSwitchValue() {
	unsigned res = *SWITCH_ADDRESS;
	return uint16_t(res);
}

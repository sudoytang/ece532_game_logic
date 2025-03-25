#ifndef SRC_CONTROLLER_TIMER_HPP_
#define SRC_CONTROLLER_TIMER_HPP_
#ifdef __MICROBLAZE__

#include <cstdint>
#include "xparameters.h"
#include "xil_exception.h"
#include "xintc.h"
#include "xtmrctr.h"

struct TimerManager {
	XTmrCtr instance;
	static constexpr int TIMER_LOAD_VALUE = 100000;  // 1ms for 100MHz clock
	uint32_t count;
	void (*TimerInterruptHandler)(void*, u8) = +[](void *CallBackRef, u8 TmrCtrNumber) {
	    XTmrCtr *TimerInstancePtr = (XTmrCtr *)CallBackRef;
	    xil_printf("interrupt: ");
	    if (XTmrCtr_IsExpired(TimerInstancePtr, TmrCtrNumber)) {
	        xil_printf("Timer Interrupt!\n");
	        XTmrCtr_Reset(TimerInstancePtr, TmrCtrNumber);
	    }
	};

	int SetupTimer() {
		auto DeviceId = XPAR_TMRCTR_0_DEVICE_ID;
	    int Status;
	    auto TimerInstancePtr = &instance;
	    Status = XTmrCtr_Initialize(TimerInstancePtr, DeviceId);
	    if (Status != XST_SUCCESS) return XST_FAILURE;

	    XTmrCtr_SetHandler(TimerInstancePtr, TimerInterruptHandler, TimerInstancePtr);
	    XTmrCtr_SetOptions(TimerInstancePtr, 0, XTC_DOWN_COUNT_OPTION | XTC_INT_MODE_OPTION | XTC_AUTO_RELOAD_OPTION);
	    XTmrCtr_SetResetValue(TimerInstancePtr, 0, TIMER_LOAD_VALUE);

	    return XST_SUCCESS;
	}
};


#endif /* __MICROBLAZE__ */
#endif /* SRC_CONTROLLER_TIMER_HPP_ */

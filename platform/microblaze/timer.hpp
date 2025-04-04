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
	static constexpr int TIMER_LOAD_VALUE = 10000000;  // 100ms for 100MHz clock
	std::array<void (*)(void*), 32> timer_callbacks;
	std::array<void*, 32> timer_callbacks_payload;
	uint32_t timer_callback_n = 0;
	void RegisterTimerCallBack(void (*cb)(void*), void* payload) {
		timer_callbacks[timer_callback_n] = cb;
		timer_callbacks_payload[timer_callback_n] = payload;
		timer_callback_n++;
	}

	void (*TimerInterruptHandler)(void*, u8) = +[](void *CallBackRef, u8 TmrCtrNumber) {
		TimerManager* man = (TimerManager*)CallBackRef;
	    XTmrCtr *TimerInstancePtr = &man->instance;
	    if (XTmrCtr_IsExpired(TimerInstancePtr, TmrCtrNumber)) {
	        XTmrCtr_Reset(TimerInstancePtr, TmrCtrNumber);
	    }
	    for (int i = 0; i < man->timer_callback_n; i++) {
	    	man->timer_callbacks[i](man->timer_callbacks_payload[i]);
	    }
	};

	int SetupTimer() {
		auto DeviceId = XPAR_TMRCTR_0_DEVICE_ID;
	    int Status;
	    auto TimerInstancePtr = &instance;
	    Status = XTmrCtr_Initialize(TimerInstancePtr, DeviceId);
	    if (Status != XST_SUCCESS) return XST_FAILURE;

	    XTmrCtr_SetHandler(TimerInstancePtr, TimerInterruptHandler, this);
	    XTmrCtr_SetOptions(TimerInstancePtr, 0, XTC_DOWN_COUNT_OPTION | XTC_INT_MODE_OPTION | XTC_AUTO_RELOAD_OPTION);
	    XTmrCtr_SetResetValue(TimerInstancePtr, 0, TIMER_LOAD_VALUE);
	    XTmrCtr_Start(TimerInstancePtr, 0);
	    return XST_SUCCESS;
	}
};


#endif /* __MICROBLAZE__ */
#endif /* SRC_CONTROLLER_TIMER_HPP_ */

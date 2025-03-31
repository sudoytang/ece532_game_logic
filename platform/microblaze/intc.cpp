#include <cstdio>
#include "intc.hpp"
#ifdef __MICROBLAZE__

constexpr size_t MAX_INTR_NUMBER = 32;
XIntc intc;


static std::array<Intr, MAX_INTR_NUMBER> intrs;
static size_t intr_size = 0;

int intrRegister(Intr intr) {
	if (intr_size == MAX_INTR_NUMBER) {
		xil_printf("Unable to register intr: Full.\n");
		return -1;
	}
	intrs[intr_size] = intr;
	intr_size++;
	return 0;
}


int init_intc() {
	XIntc *IntcInstancePtr =&intc;
	int Status;
	xil_printf("Start init intc.\n");

	Status = XIntc_Initialize(IntcInstancePtr, XPAR_INTC_0_DEVICE_ID);
	if (Status != XST_SUCCESS) {

		xil_printf( "Failed init intc\r\n");
		return XST_FAILURE;
	}
	int failedStatus = XST_SUCCESS;
	for (size_t i = 0; i < intr_size; i++) {
		auto [vec_id, handler, payload] = intrs[i];
		Status = XIntc_Connect(IntcInstancePtr, vec_id, handler, payload);
		if (Status != XST_SUCCESS) {
			xil_printf("Failed to Connect INTR[%u]: %d!\n", vec_id, Status);
			failedStatus = Status;
		} else {
			xil_printf("INTR[%u] connected to %x.\n", vec_id, handler);
		}
	}
	xil_printf("All specified INTRs connected.\n");
	if (failedStatus != XST_SUCCESS) {
		return failedStatus;
	}
	/* Start the interrupt controller */
	Status = XIntc_Start(IntcInstancePtr, XIN_REAL_MODE);
	if (Status != XST_SUCCESS) {

		xil_printf( "Failed to start intc\r\n");
		return XST_FAILURE;
	}
	xil_printf("Interrupt controller started.\n");
	/* Enable interrupts from the hardware */

	for (size_t i = 0; i < intr_size; i++) {
		auto vec_id = std::get<0>(intrs[i]);
		XIntc_Enable(IntcInstancePtr, vec_id);
		xil_printf("INTR[%u] enabled.\n", vec_id);
	}
	xil_printf("All specified intr enabled.\n");
	Xil_ExceptionInit();
	xil_printf("[Xil_ExceptionInit] done\n");
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
			(Xil_ExceptionHandler)XIntc_InterruptHandler,
			(void *)IntcInstancePtr);
	xil_printf("[Xil_ExceptionRegisterHandler] done\n");
	volatile unsigned* interrupt_vector = (unsigned*)0x10;
	xil_printf("Interrupt Vector: %x\n", *interrupt_vector);
	Xil_ExceptionEnable();
	xil_printf("End init intc.\n");
	return XST_SUCCESS;
}


#endif


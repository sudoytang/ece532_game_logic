/*
 * keyboard.hpp
 *
 *  Created on: Mar 31, 2025
 *      Author: lunar
 */

#ifndef SRC_PLATFORM_MICROBLAZE_KEYBOARD_HPP_
#define SRC_PLATFORM_MICROBLAZE_KEYBOARD_HPP_

#include "xparameters.h"
#include "axi_ps2.h"

#include "intc.hpp"

#define PS2_DEVICE_ID		XPAR_AXI_PS2_0_DEVICE_ID

#define KEYBOARD_INTR_ID	XPAR_MICROBLAZE_0_AXI_INTC_AXI_PS2_0_PS2_INTERRUPT_INTR

struct Keyboard {
	volatile int RxDataRecv = FALSE; /* Flag to indicate Receive Data */
	volatile int RxError = FALSE; 	/* Flag to indicate Receive Error */
	volatile int RxOverFlow = FALSE; /* Flag to indicate Receive Overflow */
	volatile int TxDataSent = FALSE; /* Flag to indicate Tx Data sent */
	volatile int TxNoAck = FALSE;    /* Flag to indicate Tx No Ack */
	volatile int TimeOut = FALSE;    /* Flag to indicate Watchdog Timeout */
	volatile int TxNumBytes = 0;	/* Number of bytes transmitted */
	volatile int RxNumBytes = 0;     /* Number of bytes received */
	volatile bool release = false;
	volatile bool key_w;
	volatile bool key_s;
	volatile bool key_a;
	volatile bool key_d;

	void keydecode(uint8_t raw_keycode) {
		// only decode wsad and f0 (release)
		bool val = true;
		if (release) {
			val = false;
			release = false;
		}
		switch (raw_keycode) {
		case 0x1d:
			key_w = val;
			break;
		case 0x1c:
			key_a = val;
			break;
		case 0x1b:
			key_s = val;
			break;
		case 0x23:
			key_d = val;
			break;
		case 0xf0:
			release = true;
			break;
		default:
			break;
		}
	}

	axi_ps2 Ps2Inst;
	axi_ps2_Handler Ps2IntrHandler = +[](void *CallBackRef, u32 IntrMask, u32 ByteCount) {
		Keyboard* kb_inst = (Keyboard*)CallBackRef;

//		xil_printf("@");
		axi_ps2* Ps2Ptr = &kb_inst->Ps2Inst;

		if (IntrMask & axi_ps2_IPIXR_RX_FULL) {
			/*
			 * Data is Received.
			 */
			if (kb_inst->RxDataRecv == FALSE) {
				u8 k = axi_ps2_RecvByte(Ps2Ptr->Ps2Config.BaseAddress);
				kb_inst->keydecode(k);
//				xil_printf("<%x>", k);
			}
		}

		if (IntrMask & axi_ps2_IPIXR_RX_ERR) {
			/*
			 * Receive Error.
			 */
			xil_printf ("\r\nRxError");
			kb_inst->RxError = TRUE;
		}

		if (IntrMask & axi_ps2_IPIXR_RX_OVF) {
			/*
			 * Receive overflow.
			 */
			xil_printf ("\r\nRxOverFlow");
			kb_inst->RxOverFlow = TRUE;
		}

		if (IntrMask & axi_ps2_IPIXR_TX_ACK) {
			/*
			 * Transmission of the specified data is completed.
			 */
	//		xil_printf ("\r\nBytesSent");
			kb_inst->TxDataSent = TRUE;
			kb_inst->TxNumBytes = ByteCount;
		}

		if (IntrMask & axi_ps2_IPIXR_TX_NOACK) {
			/*
			 * Transmit NO ACK.
			 */
			xil_printf ("\r\nNACK");
			kb_inst->TxNoAck = TRUE;
		}

		if (IntrMask & axi_ps2_IPIXR_WDT_TOUT) {
			/*
			 * Transmit Timeout.
			 */
			kb_inst->TimeOut = TRUE;
		}
	};

	int init() {
		xil_printf("Initialize Keyboard\n");
		int Status;
		axi_ps2_Config *ConfigPtr;

		/*
		 * Initialize the PS/2 driver.
		 */
		ConfigPtr = axi_ps2_LookupConfig(XPAR_AXI_PS2_0_DEVICE_ID);
		if (ConfigPtr == NULL) {
			xil_printf("Initialize Keyboard FAILED!\n");
			return XST_FAILURE;
		}

		axi_ps2_CfgInitialize(&Ps2Inst, ConfigPtr, ConfigPtr->BaseAddress);

		Status = axi_ps2_SelfTest(&Ps2Inst);
		if (Status != XST_SUCCESS) {
			xil_printf("Selftest Keyboard FAILED!\n");
			return XST_FAILURE;
		}


		/*
		 * Setup the interrupt system.
		 */
		intrRegister({KEYBOARD_INTR_ID,
			(XInterruptHandler)axi_ps2_IntrHandler,
			(void*)&Ps2Inst});
		axi_ps2_SetHandler(&Ps2Inst, (axi_ps2_Handler)Ps2IntrHandler, this);

		axi_ps2_IntrEnable(&Ps2Inst, axi_ps2_IPIXR_RX_ALL);
		axi_ps2_IntrGlobalEnable(&Ps2Inst);
	}
};




#endif /* SRC_PLATFORM_MICROBLAZE_KEYBOARD_HPP_ */

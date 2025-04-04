/*
 * adma.hpp
 *
 *  Created on: Apr 1, 2025
 *      Author: lunar
 */

#ifndef SRC_PLATFORM_MICROBLAZE_ADMA_HPP_
#define SRC_PLATFORM_MICROBLAZE_ADMA_HPP_

#include "xaxidma.h"
#include "xparameters.h"

struct ADMAManager {
	XAxiDma dma_inst;
	uint8_t* volatile sample_buffer;
	volatile unsigned int buffer_size;
	volatile unsigned current_idx;
	const unsigned step = 896;
	bool filling = true;

	volatile bool insert_mode = false;
	uint8_t* volatile insert_buffer;
	unsigned volatile insert_size;
	unsigned volatile insert_idx;

	volatile bool started = false;

	volatile unsigned* const adma_fifo_status = (unsigned*)(XPAR_AXI_GPIO_GYROCON_BASEADDR + 0x8);
	static constexpr unsigned PROG_EMPTY_MASK = 0b01u;
	static constexpr unsigned PROG_FULL_MASK  = 0b10u;

	int init() {
		filling = true;
		int Status;
		XAxiDma_Config *CfgPtr;
		/* Initialize the XAxiDma device.
		 */
		CfgPtr = XAxiDma_LookupConfig(XPAR_AXIDMA_0_DEVICE_ID);
		if (!CfgPtr) {
			xil_printf("No config found for %d\r\n", XPAR_AXIDMA_0_DEVICE_ID);
			return XST_FAILURE;
		}

		Status = XAxiDma_CfgInitialize(&dma_inst, CfgPtr);
		if (Status != XST_SUCCESS) {
			xil_printf("Initialization failed %d\r\n", Status);
			return XST_FAILURE;
		}

		/* Disable interrupts, we use polling mode
		 */
		XAxiDma_IntrDisable(&dma_inst, XAXIDMA_IRQ_ALL_MASK,
							XAXIDMA_DMA_TO_DEVICE);
		xil_printf("ADMA Init finished.\n");
		return XST_SUCCESS;
	}
	void setBGM(uint8_t* samples, unsigned buffer_n) {
		buffer_size = buffer_n;
		sample_buffer = samples;
		current_idx = 0;
	}

	void start() {
		started = true;
	}

	void pause() {
		started = false;
	}

	int poll_insert() {
		int status;
		if (!(XAxiDma_ReadReg(dma_inst.TxBdRing.ChanBase,
			XAXIDMA_SR_OFFSET) & XAXIDMA_HALTED_MASK)
		&& XAxiDma_Busy(&dma_inst, XAXIDMA_DMA_TO_DEVICE)) {
			xil_printf("ADMA is busy!\n");
			return XST_DEVICE_BUSY;
		}

		if (filling) {
			if (*adma_fifo_status & PROG_FULL_MASK) {
				// full!
				filling = false;
				return XST_DEVICE_BUSY;
			} else {
				status = XAxiDma_SimpleTransfer(&dma_inst, (UINTPTR)(insert_buffer + insert_idx), step, XAXIDMA_DMA_TO_DEVICE);
				insert_idx += step;
				if (insert_idx >= insert_size) {
					insert_mode = false;
				}
				current_idx += step;
				if (current_idx >= buffer_size) {
					current_idx = 0;
				}
				if (status != XST_SUCCESS) {
					xil_printf("ADMA Polling Failed: %d!!\n", status);
					return status;
				}
				return XST_SUCCESS;
			}
		} else {
			if (*adma_fifo_status & PROG_EMPTY_MASK) {
				// empty!
				filling = true;
				status = XAxiDma_SimpleTransfer(&dma_inst, (UINTPTR)(insert_buffer + insert_idx), step, XAXIDMA_DMA_TO_DEVICE);
				insert_idx += step;
				if (insert_idx >= insert_size) {
					insert_mode = false;
				}
				current_idx += step;
				if (current_idx >= buffer_size) {
					current_idx = 0;
				}
				if (status != XST_SUCCESS) {
					xil_printf("ADMA Polling Failed: %d!!\n", status);
					return status;
				}
				return XST_SUCCESS;
			} else {
				return XST_DEVICE_BUSY;
			}
		}
	}

	int poll() {
		if (!started) {
			return XST_DEVICE_IS_STOPPED;
		}
		if (insert_mode) {
			return poll_insert();
		}
		int status;
		if (!(XAxiDma_ReadReg(dma_inst.TxBdRing.ChanBase,
			XAXIDMA_SR_OFFSET) & XAXIDMA_HALTED_MASK)
		&& XAxiDma_Busy(&dma_inst, XAXIDMA_DMA_TO_DEVICE)) {
			xil_printf("ADMA is busy!\n");
			return XST_DEVICE_BUSY;
		}

		if (filling) {
			if (*adma_fifo_status & PROG_FULL_MASK) {
				// full!
				xil_printf("ADMA FIFO Full!\n");
				filling = false;
				return XST_DEVICE_BUSY;
			} else {
				xil_printf("ADMA Send\n");
				status = XAxiDma_SimpleTransfer(&dma_inst, (UINTPTR)(sample_buffer + current_idx), step, XAXIDMA_DMA_TO_DEVICE);
				current_idx += step;
				if (current_idx >= buffer_size) {
					current_idx = 0;
				}
				if (status != XST_SUCCESS) {
					xil_printf("ADMA Polling Failed: %d!!\n", status);
					return status;
				}
				return XST_SUCCESS;
			}
		} else {
			if (*adma_fifo_status & PROG_EMPTY_MASK) {
				// empty!
				xil_printf("ADMA FIFO Empty!\n");
				filling = true;
				status = XAxiDma_SimpleTransfer(&dma_inst, (UINTPTR)(sample_buffer + current_idx), step, XAXIDMA_DMA_TO_DEVICE);
				current_idx += step;
				if (current_idx >= buffer_size) {
					current_idx = 0;
				}
				if (status != XST_SUCCESS) {
					xil_printf("ADMA Polling Failed: %d!!\n", status);
					return status;
				}
				return XST_SUCCESS;
			} else {
				xil_printf("ADMA Wait\n");
				return XST_DEVICE_BUSY;
			}
		}
	}

	void insert(uint8_t* insertion_samples, unsigned length) {
		insert_mode = true;
		insert_buffer = insertion_samples;
		insert_size = length;
		insert_idx = 0;
	}
};



#endif /* SRC_PLATFORM_MICROBLAZE_ADMA_HPP_ */

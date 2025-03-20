/*
 * vdma.hpp
 *
 *  Created on: Feb 22, 2025
 *      Author: lunar
 */

#ifdef __MICROBLAZE__
#ifndef VDMA_HPP
#define VDMA_HPP

#include "xparameters.h"
#include "xaxivdma.h"

#include "../../graphics/display/display.hpp"
#include "../../graphics/image/image.hpp"

struct DisplayManager {
	static constexpr int H_MULTIPLE = 1;
	static constexpr int V_MULTIPLE = 1;
	Display800x600 display;
	XAxiVdma dma_inst;
	Image frame[Display800x600::NBUF];
	int vdma_id;
	volatile int last_written_fid = 0;
	volatile int reading_fid = 0;
	volatile int writing_fid = -1;

	volatile int total_frame_count = 0;

	int ReadSwapBuffer() {
		if (last_written_fid == writing_fid) {
			// writer should not write to the place that most recently written
			// I think this cannot happen at all, make program stuck here
			xil_printf("Read Swap Buffer Error!!\n");
			while (true) {}
		}

//		if (vdma_id == 1) if (last_written_fid != reading_fid)
//		xil_printf("VDMA#%d current fid = %d, last written = %d, writing = %d\n",vdma_id, reading_fid, last_written_fid, writing_fid);
		// we choose the newest written buffer to display.
		if (reading_fid == last_written_fid) {
			// no need to swap
			if (vdma_id == 1) {
//				xil_printf("#%d, No need to swap. @ #%d\n", vdma_id, reading_fid);
			}
			return 0;
		}

		reading_fid = last_written_fid;

		return XAxiVdma_StartParking(&dma_inst, reading_fid, XAXIVDMA_READ);
	}
	void StartWritingBuffer(int fid) {
		writing_fid = fid;
//		xil_printf("#%d Start writing buffer %d\n", vdma_id, fid);
	}
	void EndWritingBuffer() {
		writing_fid = -1;
//		xil_printf("#%d End writing buffer\n", vdma_id);
	}


	int GetBestFIDForDrawing() const {
		for (int i = 0; i < Display800x600::NBUF; i++) {
			if (reading_fid == i) continue;
			if (last_written_fid == i) continue;
			return i;
		}
		xil_printf("WARNING: No Frame is suitable for drawing.\n");
		return (reading_fid + 1 % Display800x600::NBUF);
	}
	int StartVDMA() {
		return XAxiVdma_DmaStart(&dma_inst, XAXIVDMA_READ);
	}


	struct PayloadType {
		DisplayManager* manager;
		int status;
	} payload;

	DisplayCB::FuncPtr swap_cb_fn = +[](Display800x600* disp, void* payload, void* sys_payload) {

		(void)sys_payload;  // silence compiler
		auto fid = disp->getCurrentFID();
		auto pld = (DisplayManager::PayloadType*)payload;
		pld->manager->last_written_fid = fid;
//		if (pld->manager->vdma_id == 1)
//		xil_printf("[SWAP_CB] #%d Set Last Written buffer = %d\n", pld->manager->vdma_id, fid);
		pld->manager->ReadSwapBuffer();
		return;
	};

	DisplayCB::FuncPtr point_cb_fn = +[](Display800x600* disp, void* payload, void* sys_payload) {
		auto point = (std::pair<int, int>*)sys_payload;
		auto pld = (DisplayManager::PayloadType*)payload;
		auto vdma = &pld->manager->dma_inst;
		constexpr int H_MAX_OFFSET = (DisplayManager::H_MULTIPLE - 1) * Display800x600::HRES;
		constexpr int V_MAX_OFFSET = (DisplayManager::V_MULTIPLE - 1) * Display800x600::VRES;
		auto x = point->first;
		auto y = point->second;
		bool invalid_op = false;
		if (x < 0) {
			// invalid point-to operation
			invalid_op = true;
			x = 0;
		}
		if (x > H_MAX_OFFSET) {
			invalid_op = true;
			x = H_MAX_OFFSET;
		}
		if (y < 0) {
			// invalid point-to operation
			invalid_op = true;
			y = 0;
		}
		if (y > V_MAX_OFFSET) {
			invalid_op = true;
			y = V_MAX_OFFSET;
		}
		auto fid = disp->getCurrentFID();
		if (invalid_op) {
			pld->status = XST_FAILURE;
			// WARNING: this is a recursive function, be careful
			// check here if anything unexpected happens
			disp->pointScreen(fid, x, y);
			return;
		}
		pld->status = DisplayManager::dmaResetCurrentScreenPtr(disp, vdma);
		return;
	};

	DisplayCB::FuncPtr reload_cb_fn = +[](Display800x600* disp, void* payload, void* sys_payload) {
		auto pld = (DisplayManager::PayloadType*)payload;
		auto vdma = &pld->manager->dma_inst;
		pld->status = DisplayManager::dmaResetCurrentScreenPtr(disp, vdma);
		return;
	};



	static Color* getFirstPixelAddress(ImageSlice slice) {
		auto ptr = slice.view.data;
		ptr += (slice.y * slice.view.width);
		ptr += slice.x;
		return ptr;
	}

	static Color* getEndPixelAddress(ImageSlice slice) {
		auto ptr = slice.view.data;
		ptr += ((slice.y + slice.height) * slice.view.width);
		return ptr;
	}

	static int dmaResetCurrentScreenPtr(Display800x600* disp, XAxiVdma* vdma) {
		void* ptrs[Display800x600::NBUF];
		for (int i = 0; i < Display800x600::NBUF; i++) {
			auto slice = disp->getScreen(i);
			ptrs[i] = DisplayManager::getFirstPixelAddress(slice);
		}
		int status = XAxiVdma_DmaSetBufferAddr(vdma, XAXIVDMA_READ, (UINTPTR*)&ptrs);
		if (status != XST_SUCCESS) {
			return status;
		}
		status = XAxiVdma_DmaStart(vdma, XAXIVDMA_READ);
		return status;
	}

	int init() {
		payload.status = 0;
		payload.manager = this;
		xil_printf("Initializing Display Manager for VDMA #%d...\n", vdma_id);
		constexpr int NBUF = Display800x600::NBUF;
		void* framep[NBUF];

		for (int i = 0; i < NBUF; i++) {
			frame[i] = Image::fromBlank(display.HRES * H_MULTIPLE, display.VRES * V_MULTIPLE);
			framep[i] = frame[i].data.get();
			xil_printf("frame #%d is @%x\n", i, framep[i]);
		}
		display = Display800x600(H_MULTIPLE, V_MULTIPLE, framep);
		DisplayCB swap_cb;
		swap_cb.payload = (void*)&payload;
		swap_cb.func = swap_cb_fn;
		display.registerSwapCallBack(swap_cb);
		DisplayCB point_cb;
		point_cb.payload = (void*)&payload;
		point_cb.func = point_cb_fn;
		display.registerPointCallBack(point_cb);
		DisplayCB reload_cb;
		reload_cb.payload = (void*)&payload;
		reload_cb.func = reload_cb_fn;
		display.registerReloadCallBack(reload_cb);


		int status;
		XAxiVdma_Config* config = XAxiVdma_LookupConfig(vdma_id);
		if (!config) {
			xil_printf("No video DMA found for ID %d\r\n", vdma_id);
			return XST_FAILURE;
		}
		/* Initialize DMA engine */
		status = XAxiVdma_CfgInitialize(&dma_inst, config, config->BaseAddress);
		if (status != XST_SUCCESS) {
			xil_printf("Configuration Initialization failed, status: 0x%X\r\n", status);
			return status;
		}

		XAxiVdma_FrameCounter FrameCfg;
		FrameCfg.ReadFrameCount = 1;
		FrameCfg.ReadDelayTimerCount = 10;

		// we don't use/have write channel but XAxiVdma_SetFrameCounter need both
		// config to be valid
		FrameCfg.WriteFrameCount = 1;
		FrameCfg.WriteDelayTimerCount = 10;

		status = XAxiVdma_SetFrameCounter(&dma_inst, &FrameCfg);
		if (status != XST_SUCCESS) {
			xil_printf(
				"Set frame counter failed %d\r\n", status);

			if(status == XST_VDMA_MISMATCH_ERROR)
				xil_printf("DMA Mismatch Error\r\n");

			return XST_FAILURE;
		}

		uint32_t horizontal_size = display.HRES * (config->Mm2SStreamWidth>>3);
		xil_printf("Horizontal size: %lu\n", horizontal_size);
		uint32_t stride = H_MULTIPLE * horizontal_size;
		XAxiVdma_DmaSetup read_cfg;
		read_cfg.VertSizeInput       = display.VRES;	// vertical video size
		read_cfg.HoriSizeInput       = horizontal_size;	// horizontal video size
		read_cfg.Stride              = stride;			// horizontal video stride
														// useful when the frame_buffer is bigger than screen
		read_cfg.FrameDelay          = 0;      		// ?
		read_cfg.EnableCircularBuf   = 0;      			// we swap buffer manually
		read_cfg.EnableSync          = 0;      			// ? no gen_loc
		read_cfg.PointNum            = 0;				// ? no need to sync master
		read_cfg.EnableFrameCounter  = 0;      			// stop vdma after 1 frame
		read_cfg.FixedFrameStoreAddr = 0;      			// no need to store frame

		status = XAxiVdma_DmaConfig(&dma_inst, XAXIVDMA_READ, &read_cfg);
		if (status != XST_SUCCESS) {
			xil_printf("Read channel config failed, status: 0x%X\r\n", status);
			return status;
		}

		XAxiVdma_Channel *channel  = XAxiVdma_GetChannel(&dma_inst, XAXIVDMA_READ);
		xil_printf("Channel word length mask: %x\n", (u32)(channel->WordLength - 1));
		xil_printf("Channel Frames Number: %d\n", channel->NumFrames);
		status = XAxiVdma_DmaSetBufferAddr(&dma_inst, XAXIVDMA_READ, (UINTPTR *) &framep);
		if (status != XST_SUCCESS) {
			xil_printf("Read channel set buffer address failed, status: 0x%X\r\n", status);
			return status;
		}
		auto onRead = +[](void* cb_ref, u32 mask) {
			DisplayManager* manager = (DisplayManager*)cb_ref;
			manager->total_frame_count += 1;
//			manager->ReadSwapBuffer();
//			manager->StartVDMA();
		};
		status = XAxiVdma_SetCallBack(&dma_inst, XAXIVDMA_HANDLER_GENERAL, (void*)onRead,
		    (void *)this, XAXIVDMA_READ);
		if (status != XST_SUCCESS) {
			xil_printf("Set read callback failed: 0x%X\r\n", status);
			return status;
		}
		auto onErr = +[](void* cb_ref, u32 mask) {
			xil_printf("[INTR] VDMA Error!\n");
		};
		status = XAxiVdma_SetCallBack(&dma_inst, XAXIVDMA_HANDLER_ERROR,
		    (void*)onErr, (void *)this, XAXIVDMA_READ);
		if (status != XST_SUCCESS) {
			xil_printf("Set error callback failed: 0x%X\r\n", status);
			return status;
		}
		XAxiVdma_IntrEnable(&dma_inst, XAXIVDMA_IXR_ALL_MASK, XAXIVDMA_READ);
		status = XAxiVdma_DmaStart(&dma_inst, XAXIVDMA_READ);
		if (status != XST_SUCCESS) {
			xil_printf("Failed to start DMA engine (read channel), status: 0x%X\r\n", status);
			return status;
		}

		// Set Auto-Swap buffer:


		xil_printf("Successfully Initialized Display Manager.\n");
		return XST_SUCCESS;
	}
};



#endif // VDMA_HPP
#endif // __MICROBLAZE__

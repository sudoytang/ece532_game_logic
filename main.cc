/*
 * Empty C++ Application
 */

#include <cstdio>
#include <utility>
#include <climits>
#include <cassert>

#include "memory/memory.hpp"
#include "graphics/font/font.hpp"

#include "game/map.hpp"
#include "game/car.hpp"

#include "controller/gyro.hpp"
#include "controller/timer.hpp"

#ifdef __MICROBLAZE__
#include "platform/microblaze/vdma.hpp"
#include "xparameters.h"
#include "xil_cache.h"
#include "xintc.h"
#endif
#ifdef _WIN32
#include "platform/win32/tcp_client.hpp"
#endif

void (*softReset)(void) = nullptr;

#define CHK_STATUS(status)													\
if (status != XST_SUCCESS) { 												\
	xil_printf("Status Error <%d> in %s:%d\n", status, __FILE__, __LINE__);	\
	return status;															\
}

DisplayManager m_bg;
DisplayManager m_fg;

XIntc intc;

GYROManager gyro;
TimerManager timer;

int init_intc() {
	XIntc *IntcInstancePtr =&intc;
	int Status;
	xil_printf("Start init intc.\n");
	/* Initialize the interrupt controller and connect the ISRs */
	Status = XIntc_Initialize(IntcInstancePtr, XPAR_INTC_0_DEVICE_ID);
	if (Status != XST_SUCCESS) {

		xil_printf( "Failed init intc\r\n");
		return XST_FAILURE;
	}

	Status = XIntc_Connect(IntcInstancePtr, XPAR_INTC_0_AXIVDMA_0_VEC_ID,
	         (XInterruptHandler)XAxiVdma_ReadIntrHandler, &m_bg.dma_inst);
	if (Status != XST_SUCCESS) {
		xil_printf(
		    "Failed read channel connect intc %d\r\n", Status);
		return XST_FAILURE;
	}
	Status = XIntc_Connect(IntcInstancePtr, XPAR_INTC_0_AXIVDMA_1_VEC_ID,
	         (XInterruptHandler)XAxiVdma_ReadIntrHandler, &m_fg.dma_inst);
	if (Status != XST_SUCCESS) {
		xil_printf(
		    "Failed read channel connect intc %d\r\n", Status);
		return XST_FAILURE;
	}
    Status = XIntc_Connect(IntcInstancePtr, XPAR_INTC_0_TMRCTR_0_VEC_ID,
    		 (XInterruptHandler)XTmrCtr_InterruptHandler, &timer.instance);
	if (Status != XST_SUCCESS) {
		xil_printf(
		    "Failed timer connect intc %d\r\n", Status);
		return XST_FAILURE;
	}
	/* Start the interrupt controller */
	Status = XIntc_Start(IntcInstancePtr, XIN_REAL_MODE);
	if (Status != XST_SUCCESS) {

		xil_printf( "Failed to start intc\r\n");
		return XST_FAILURE;
	}

	/* Enable interrupts from the hardware */
	XIntc_Enable(IntcInstancePtr, XPAR_INTC_0_AXIVDMA_0_VEC_ID);
	XIntc_Enable(IntcInstancePtr, XPAR_INTC_0_AXIVDMA_1_VEC_ID);
	XIntc_Enable(IntcInstancePtr, XPAR_INTC_0_TMRCTR_0_VEC_ID);

	Xil_ExceptionInit();
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
			(Xil_ExceptionHandler)XIntc_InterruptHandler,
			(void *)IntcInstancePtr);

	Xil_ExceptionEnable();
	xil_printf("End init intc.\n");
	return XST_SUCCESS;
}

void do_mem_test() {
	// do some simple self tests
	void* mem = allocate(100);
	deallocate(mem);
	xil_printf("allocate 1 successful.\n");
	mem = allocate_aligned(100, 16);
	deallocate(mem);
	xil_printf("allocate 2 successful.\n");
	mem = allocate_vram(128);
	deallocate_vram(mem);
	xil_printf("allocate vram successful.\n");
	xil_printf("memory test ok!\n");
}


int init_all() {
	Xil_ICacheEnable();
	Xil_DCacheEnable();
	int status;
	status = memory_init();
	CHK_STATUS(status);
	Font::initDefaultFont();

	do_mem_test();

	m_bg.vdma_id = XPAR_VGA_SYSTEM_AXI_VDMA_0_DEVICE_ID;
	m_fg.vdma_id = XPAR_VGA_SYSTEM_AXI_VDMA_SPRITE_DEVICE_ID;

	status = m_bg.init();
	CHK_STATUS(status);
	status = m_fg.init();
	CHK_STATUS(status);

	status = timer.SetupTimer();
	CHK_STATUS(status);

	status = init_intc();
	CHK_STATUS(status);
	gyro.init();
	gyro.calib();
	return XST_SUCCESS;
}

constexpr int height = Display800x600::VRES;
constexpr int width = Display800x600::HRES;



int main_1()
{
	xil_printf("\n\nHello World!\n");
	int status = init_all();
	CHK_STATUS(status);
	const Color DARK_BLUE = 0xf910;
	const Color LIGHT_YELLOW = 0xf5ef;

	Draw draw;

//	auto fid = m_bg.GetBestFIDForDrawing();
	auto fid = m_bg.display.getNextFID();
	m_bg.StartWritingBuffer(fid);
    draw = m_bg.display.getScreenDraw(fid);
    draw.setBlank(DARK_BLUE);
    draw.drawCircle(100, 100, 50, LIGHT_YELLOW, true);
    draw.drawCircle(600, 200, 25, C_WHITE, true);
    draw.drawCircle(600 - 25, 200 - 25, 25, DARK_BLUE, true);
    draw.drawCircle(600 - 25, 200 + 25, 25, DARK_BLUE, true);
    draw.drawCircle(600 + 25, 200 - 25, 25, DARK_BLUE, true);
    draw.drawCircle(600 + 25, 200 + 25, 25, DARK_BLUE, true);
    m_bg.EndWritingBuffer();
    m_bg.display.swapBuffers();
    // draw #2
//	fid = m_bg.GetBestFIDForDrawing();
    fid = m_bg.display.getNextFID();
	m_bg.StartWritingBuffer(fid);
	m_bg.frame[fid].setBlank(DARK_BLUE);
    draw = m_bg.display.getScreenDraw(fid);
	draw.drawCircle(100, 100, 50, LIGHT_YELLOW, true);
	draw.drawCircle(600, 200, 25, C_WHITE, true);
	draw.drawCircle(600 - 25, 200 - 25, 25, DARK_BLUE, true);
	draw.drawCircle(600 - 25, 200 + 25, 25, DARK_BLUE, true);
	draw.drawCircle(600 + 25, 200 - 25, 25, DARK_BLUE, true);
	draw.drawCircle(600 + 25, 200 + 25, 25, DARK_BLUE, true);
    m_bg.EndWritingBuffer();
	m_bg.display.swapBuffers();
	// draw #0
//	fid = m_bg.GetBestFIDForDrawing();
	fid = m_bg.display.getNextFID();
	m_bg.frame[fid].setBlank(DARK_BLUE);
    draw = m_bg.display.getScreenDraw(fid);
	draw.drawCircle(100, 100, 50, LIGHT_YELLOW, true);
	draw.drawCircle(600, 200, 25, C_WHITE, true);
	draw.drawCircle(600 - 25, 200 - 25, 25, DARK_BLUE, true);
	draw.drawCircle(600 - 25, 200 + 25, 25, DARK_BLUE, true);
	draw.drawCircle(600 + 25, 200 - 25, 25, DARK_BLUE, true);
	draw.drawCircle(600 + 25, 200 + 25, 25, DARK_BLUE, true);
    m_bg.EndWritingBuffer();
	m_bg.display.swapBuffers();

	char hello_s[] = "Hello_World! 0 0000000000";
	const int size = sizeof(hello_s);


    int draw_x = 100;
    int draw_y = 500;
    int step = 3;
    int direction_x = step;
    int direction_y = step;
    std::pair<int, int> restore_pos[Display800x600::NBUF];
    for (int i = 0; i < Display800x600::NBUF; i++) {
        restore_pos[i] = {INT_MIN, INT_MIN};
    }

    while (true) {
//        auto next_fid = m_fg.GetBestFIDForDrawing();
    	auto next_fid = m_fg.display.getNextFID();
//        xil_printf("Drawing to frame %d\n", next_fid);
        m_fg.StartWritingBuffer(next_fid);
        draw = m_fg.display.getScreenDraw(next_fid);
        for (auto it = hello_s + size - 2; it != hello_s + size - 2 - 10; it--) {
        	auto& ch = *it;
        	if (ch == '9') {
        		ch = '0';
        	} else {
        		ch += 1;
        		break;
        	}
        }
        auto p = hello_s + size - 2 - 10 - 1;
        *p = '0' + next_fid;
//        xil_printf("hello_s = %s\n", hello_s);
    	Image hello_world = Draw::genTextImage(Font::DEFAULT_FONT, hello_s, C_WHITE);
        if (restore_pos[next_fid].first != INT_MIN) {
        	draw.drawRect(restore_pos[next_fid].first, restore_pos[next_fid].second, hello_world.width, hello_world.height, C_TRANSPARENT, true);
        }
        restore_pos[next_fid] = {draw_x, draw_y};


//        xil_printf("Drawing FID %d, screen = [%d,%d](%d,%d) @(%x) <Image @(%x)>\n",
//			next_fid, draw.getSlice().x, draw.getSlice().y, draw.getSlice().width, draw.getSlice().height,
//			draw.getSlice().view.data, m_fg.frame[next_fid].data.get());


        draw.drawImage(draw_x, draw_y, hello_world);
        auto drawn_slice = draw.getSlice().slice(draw_x, draw_y, hello_world.width, hello_world.height);
        auto start_cache_ptr = (UINTPTR)m_fg.getFirstPixelAddress(drawn_slice);
        int cache_len = (UINTPTR)m_fg.getEndPixelAddress(drawn_slice) - start_cache_ptr;
        Xil_DCacheFlushRange(start_cache_ptr, cache_len);
//        for (int i = 0; i < 10000000; i++) {
//        	if (i % 1000000 == 0) {
//        		xil_printf(".");
//        	}
//        }
        m_fg.EndWritingBuffer();
//        xil_printf("Draw on %d @%x (%d,%d). ", (int)next_fid, draw.getSlice().view.data, draw_x, draw_y);
        m_fg.display.swapBuffers(next_fid);
//        xil_printf("Swap to %d: %d", (int)m_fg.display.getCurrentFID(), m_fg.payload.status);

//        if (restore_pos[next_fid].first != INT_MIN) {
//            int restore_x = restore_pos[next_fid].first;
//            int restore_y = restore_pos[next_fid].second;
////            for (int sleep = 0; sleep < 10000000; sleep++) {
////            	if (sleep % 1000000 == 0) xil_printf("#");
////            }
//            draw.drawRect(restore_x, restore_y,
//                hello_world.width, hello_world.height, C_TRANSPARENT, true);
//            auto drawn_slice = draw.getSlice().slice(restore_x, restore_y, hello_world.width, hello_world.height);
//            auto start_cache_ptr = (UINTPTR)m_fg.getFirstPixelAddress(drawn_slice);
//            int cache_len = (UINTPTR)m_fg.getEndPixelAddress(drawn_slice) - start_cache_ptr;
//            Xil_DCacheFlushRange(start_cache_ptr, cache_len);
////            xil_printf("Clear on %d @%x (%d,%d).", next_fid, draw.getSlice().view.data, restore_x, restore_y);
////            for (int sleep = 0; sleep < 10000000; sleep++) {
////            	if (sleep % 1000000 == 0) xil_printf("@");
////            }
////            xil_printf("\n");
//        }

//        hello_world.setBlank(C_TRANSPARENT);
//        Draw::from(hello_world).drawText(Font::DEFAULT_FONT, 0, 0, hello_s, C_WHITE);

        if ((draw_x + hello_world.width) + step > width) {
            direction_x = -step;
        } else if (draw_x < step) {
            direction_x = step;
        }
        draw_x += direction_x;
        if (draw_y + hello_world.height + step >= height) {
            direction_y = -step;
        } else if (draw_y < step) {
            direction_y = step;
        }
        draw_y += direction_y;
    }
}

typedef enum {
	BTN_NONE = 0b00000,
	BTNC = 0b00001,
	BTNU = 0b00010,
	BTNL = 0b00100,
	BTNR = 0b01000,
	BTND = 0b10000,
} Button;

Map map;

int main_game() {
	xil_printf("\n\nHello World!\n");
	int status = init_all();
	CHK_STATUS(status);
	gyro.poll();
	map.init();
	map.draw(m_bg.display.getScreenDraw(0));

//	m_fg.display.getScreenDraw(0).drawText(Font::DEFAULT_FONT, 10, 10, "0", C_WHITE);
//	m_fg.display.getScreenDraw(1).drawText(Font::DEFAULT_FONT, 30, 10, "1", C_WHITE);
//	m_fg.display.getScreenDraw(2).drawText(Font::DEFAULT_FONT, 50, 10, "2", C_WHITE);
	Car car (200, 200, &map);
	std::tuple<int /*x*/, int /*y*/, int /*width*/, int /*height*/>
		restore_pos[Display800x600::NBUF];
    for (int i = 0; i < Display800x600::NBUF; i++) {
        restore_pos[i] = {INT_MIN, INT_MIN, INT_MIN, INT_MIN};
    }

    auto get_impulse_from_btn = [&car]() {
    	uint32_t btn = *(volatile uint32_t*)XPAR_MB_SYSTEM_AXI_GPIO_0_BASEADDR;
    	if (btn == 0) return;
    	const float impulse = 0.5f;
    	float impulse_x = 0.0f;
    	float impulse_y = 0.0f;

    	if (btn & BTNU) {
    		impulse_y -= impulse;
    	}
    	if (btn & BTND) {
    		impulse_y += impulse;
    	}
    	if (btn & BTNL) {
    		impulse_x -= impulse;
    	}
    	if (btn & BTNR) {
    		impulse_x += impulse;
    	}
    	if (btn & BTNC) {
    		gyro.reset();
    	}
    	car.applyImpulseCartesian(impulse_x, impulse_y);
    };

    int last_frame_count = -1;
    while (true) {
    	gyro.poll();
    	if (last_frame_count >= m_fg.total_frame_count) {
    		usleep(1000);
    		continue;
    	}
    	if (std::abs(gyro.currentX) > 4.f || std::abs(gyro.currentY) > 4.f)
			car.applyImpulseCartesian(
				- gyro.currentX * (0.3f/90.f),
				gyro.currentY * (0.3f/90.f));
    	if (m_fg.total_frame_count % 60 == 0) {
    		xil_printf("GYRO X:%d Y:%d Z:%d\n",
    			(int)gyro.currentX, (int)gyro.currentY, (int)gyro.currentZ);
    	}
//    	xil_printf("Frame count: %d\n", m_fg.total_frame_count);
    	last_frame_count = m_fg.total_frame_count;
    	get_impulse_from_btn();
    	car.update();
    	auto next_fid = m_fg.display.getNextFID();
    	auto draw = m_fg.display.getScreenDraw(next_fid);
        m_fg.StartWritingBuffer(next_fid);
        auto [x, y, xsize, ysize] = restore_pos[next_fid];
        if (x != INT_MIN) {
        	draw.drawRect(x, y, xsize, ysize, C_TRANSPARENT, true);
        }
        restore_pos[next_fid] = car.draw(draw, 0);
        auto [drw_x, drw_y, drw_xsize, drw_ysize] = restore_pos[next_fid];
        ImageSlice refresh_slice;
        if (x != INT_MIN) {
        	refresh_slice = draw.getSlice().slice(
				std::min(x, drw_x), std::min(y, drw_y),
				std::abs(x - drw_x) + drw_xsize, std::abs(y - drw_y) + drw_ysize);
        } else {
        	refresh_slice = draw.getSlice().slice(
				drw_x, drw_y,
				drw_xsize, drw_ysize);
        }
        auto start_cache_ptr = (UINTPTR)m_fg.getFirstPixelAddress(refresh_slice);
        int cache_len = (UINTPTR)m_fg.getEndPixelAddress(refresh_slice) - start_cache_ptr;
        Xil_DCacheFlushRange(start_cache_ptr, cache_len);
        m_fg.EndWritingBuffer();
        m_fg.display.swapBuffers(next_fid);
    	usleep(1000);
    }

	return 0;
}

int main_gyro() {
	gyro.init();
	gyro.calib();
	while (1) gyro.poll();
	return 0;
}

int main() {

	xil_printf("\n\nMain: Hello World!!\n");
//	auto status = init_all();
//	CHK_STATUS(status);
//	main_1();
	main_game();
//	main_gyro();
//	assert(0);
}

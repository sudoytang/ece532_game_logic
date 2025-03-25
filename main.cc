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

#include "platform/disp_man.hpp"
#ifdef __MICROBLAZE__
#include "platform/microblaze/gyro.hpp"
#include "platform/microblaze/timer.hpp"
#include "platform/microblaze/intc.hpp"
#include "xparameters.h"
#include "xil_cache.h"

GYROManager gyro0;
//GYROManager gyro1;
TimerManager timer;

DisplayManager m_bg;
DisplayManager m_fg;

void registerAllIntrs() {
	intrRegister({XPAR_INTC_0_AXIVDMA_0_VEC_ID, XAxiVdma_ReadIntrHandler, (void*)&m_bg.man.dma_inst});
	intrRegister({XPAR_INTC_0_AXIVDMA_1_VEC_ID, XAxiVdma_ReadIntrHandler, (void*)&m_fg.man.dma_inst});
}

#endif

void (*softReset)(void) = nullptr;

#define CHK_STATUS(status)													\
if (status != XST_SUCCESS) { 												\
	xil_printf("Status Error <%d> in %s:%d\n", status, __FILE__, __LINE__);	\
	return status;															\
}








void do_mem_test() {
	// do some simple self tests
	void* mem = allocate(100);
	deallocate(mem);
	xil_printf("allocate 1 successful.\n");
	mem = allocate_aligned(100, 16);
	deallocate(mem);
	xil_printf("allocate 2 successful.\n");
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

	m_bg.setDeviceId(DISPMAN_BG_DEVICE_ID);
	m_fg.setDeviceId(DISPMAN_FG_DEVICE_ID);

	status = m_bg.init();
	CHK_STATUS(status);
	status = m_fg.init();
	CHK_STATUS(status);

	status = timer.SetupTimer();
	CHK_STATUS(status);

	registerAllIntrs();
	status = init_intc();
	CHK_STATUS(status);
	gyro0.init((void*)GYRO0_RAW_ADDR, true);
	gyro0.calib();
//	gyro1.init((void*)GYRO1_RAW_ADDR, true);
//	gyro1.calib();
	return XST_SUCCESS;
}

constexpr int height = Display800x600::VRES;
constexpr int width = Display800x600::HRES;

typedef enum {
	BTN_NONE = 0b00000,
	BTNU = 0b00001,
	BTNL = 0b00010,
	BTNR = 0b00100,
	BTND = 0b01000,
	BTNC = 0b10000,
} Button;

Map map;

int main_game() {
	xil_printf("\n\nHello World!\n");
	int status = init_all();
	CHK_STATUS(status);
	gyro0.poll();
//	gyro1.poll();
	map.init();
	map.draw(m_bg.getDisplay().getScreenDraw(0));

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
    	uint32_t btn = gyro0.btn_val;
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
    		gyro0.reset();
//    		gyro1.reset();
    	}
    	car.applyImpulseCartesian(impulse_x, impulse_y);
    };

    int last_frame_count = -1;
    while (true) {
    	gyro0.poll();
//    	gyro1.poll();
    	if (last_frame_count >= m_fg.getFrameCount()) {
    		usleep(1000);
    		continue;
    	}
    	if (std::abs(gyro0.currentX) > 4.f || std::abs(gyro0.currentY) > 4.f)
			car.applyImpulseCartesian(
				- gyro0.currentX * (0.3f/90.f),
				gyro0.currentY * (0.3f/90.f));
    	if (m_fg.getFrameCount() % 60 == 0) {
    		xil_printf("GYRO X:%d Y:%d Z:%d\n",
    			(int)gyro0.currentX, (int)gyro0.currentY, (int)gyro0.currentZ);
    	}
//    	xil_printf("Frame count: %d\n", m_fg.total_frame_count);
    	last_frame_count = m_fg.getFrameCount();
    	get_impulse_from_btn();
    	car.update();
    	auto next_fid = m_fg.getDisplay().getNextFID();
    	auto draw = m_fg.getDisplay().getScreenDraw(next_fid);
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
        m_fg.man.FlushPixels(refresh_slice);

        m_fg.getDisplay().swapBuffers(next_fid);
    	usleep(1000);
    }

	return 0;
}


int main() {

	xil_printf("\n\nMain: Hello World!!\n");
//	auto status = init_all();
//	CHK_STATUS(status);
//	main_1();
	main_game();
//	assert(0);
}

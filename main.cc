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
#include "game/game.hpp"

#include "game/data/bgm.h"

#include "platform/disp_man.hpp"
#ifdef __MICROBLAZE__
#include "platform/microblaze/gyro.hpp"
#include "platform/microblaze/timer.hpp"
#include "platform/microblaze/intc.hpp"
#include "xparameters.h"
#include "xil_cache.h"
#include "platform/microblaze/sprite_engine.hpp"
#include "platform/microblaze/adma.hpp"
TimerManager timer;

DisplayManager m_bg;
DisplayManager m_fg;
ADMAManager adma;

void registerAllIntrs() {
	intrRegister({XPAR_INTC_0_AXIVDMA_0_VEC_ID, XAxiVdma_ReadIntrHandler, (void*)&m_bg.man.dma_inst});
	intrRegister({XPAR_INTC_0_AXIVDMA_1_VEC_ID, XAxiVdma_ReadIntrHandler, (void*)&m_fg.man.dma_inst});
	intrRegister({XPAR_INTC_0_TMRCTR_0_VEC_ID, XTmrCtr_InterruptHandler, (void*)&timer.instance});
}


#else

#define xil_printf printf
#define XST_SUCCESS 0
#endif

void (*softReset)(void) = nullptr;

#define CHK_STATUS(status)													\
if (status != XST_SUCCESS) { 												\
	xil_printf("Status Error <%d> in %s:%d\n", status, __FILE__, __LINE__);	\
	return status;															\
}


Game game;





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

	timer.RegisterTimerCallBack(+[](void* adma) {
		ADMAManager* inst = (ADMAManager*)adma;
		inst->poll();
	}, &adma);
	status = timer.SetupTimer();
	CHK_STATUS(status);
	game.controller.init();
	xil_printf("AMDA init.\n");
	status = adma.init();
	CHK_STATUS(status);
	adma.setBGM((uint8_t*)bgm, bgm_size);

	registerAllIntrs();
	status = init_intc();
	CHK_STATUS(status);
	xil_printf("[init_all] success.\n");
	return XST_SUCCESS;
}

constexpr int height = Display800x600::VRES;
constexpr int width = Display800x600::HRES;

uint8_t insert_samples[896 * 2];

void generate_sweep_sfx() {

    const int totalSamples = 896 * 2;  // Total number of samples
    float frequency = 1000;        // Initial frequency in arbitrary units
    double decayFactor = 0.99;      // Factor to decrease frequency
    int sampleRate = 8000;          // Samples per second

    for (int i = 0, period = sampleRate / frequency; i < totalSamples; i++) {
        // Generate square wave sample
        uint8_t sample = (i / (period / 2)) % 2 == 0 ? 254 : 1;
        insert_samples[i] = sample;

        // Gradually decrease the frequency
        if (i % (period * 2) == 0) {
            frequency *= decayFactor;
            period = sampleRate / frequency;  // Recalculate period
        }
    }
}

int main_game_final() {
	xil_printf("\n\nHello World!\n");
	int status = init_all();
	CHK_STATUS(status);
	m_bg.getDisplay().getScreenDraw(0).setBlank(C_WHITE);
	game.init();
	game.draw_static(m_bg.getDisplay().getScreenDraw(0));

	generate_sweep_sfx();

    adma.start();
	int last_frame_count = -1;
    xil_printf("Starting Game loop!\n");
	for (;;) {

		auto frame_count = m_fg.getFrameCount();
		if (last_frame_count >= frame_count) {
			usleep(1666);
			continue;
		}

		if (last_frame_count != -1 && last_frame_count + 1 < frame_count) {
			xil_printf("[WARN] Buffer underrun: %d frames skipped.\n", frame_count - (last_frame_count + 1));
		}

		auto static_redraw = game.update();

		auto next_fid = 0;
		auto draw = m_fg.getDisplay().getScreenDraw(next_fid);
		if (static_redraw) {
			adma.insert(insert_samples, 896 * 2);
			xil_printf("Static redraw\n");
			draw.setBlank(C_TRANSPARENT);
			game.draw_static(m_bg.getDisplay().getScreenDraw(0));
		}
		game.draw_dynamic(draw);
		last_frame_count = frame_count;
		usleep(1666);
	}
}

int kb_main() {
	game.controller.kb.init();
	init_intc();
	while (1) {
		usleep(10000);
	}
}


int main() {

	xil_printf("\n\nMain: Hello World!!\n");
	volatile unsigned* vectors = (unsigned*)0x00;
	xil_printf("Vectors: \n");
	for (int i = 0; i < 8; i += 2) {
		xil_printf("%x: %x %x\n", i * 4, vectors[i], vectors[i+1]);
	}
	if (vectors[0] == 0) {
		xil_printf("System corrputed!!!!!\n");
		while (true) {}
	}

//	kb_main();
//	auto status = init_all();
//	CHK_STATUS(status);
//	main_1();
//	main_game();
	main_game_final();
//	assert(0);
}

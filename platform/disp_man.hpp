#ifndef PLATFORM_DISPLAY_HPP_
#define PLATFORM_DISPLAY_HPP_

#include <array>

#include "fwd.hpp"
#include "../graphics/display/display.hpp"

#ifdef __MICROBLAZE__
#include "microblaze/vdma.hpp"
struct DM_MB_ {
	VDMAManager man;
};

#define DISPMAN_FG_DEVICE_ID XPAR_VDMA_FG0_DEVICE_ID
#define DISPMAN_BG_DEVICE_ID XPAR_VDMA_BG_DEVICE_ID

#define DISPMAN_EXTENDING DM_MB_
#else
struct DM_UNI_ {
	// TODO: implement non-microblaze behavior
};
#define DISPMAN_EXTENDING DM_UNI_
#endif



struct DisplayManager: public DISPMAN_EXTENDING {
	Display800x600& getDisplay() {
		return man.display;
	}
	void setDeviceId(int id) {
		man.setDeviceId(id);
	}
	int init() {
		return man.init();
	}
	int getFrameCount() {
		return man.total_frame_count;
	}
};



#endif // PLATFORM_DISPLAY_HPP_

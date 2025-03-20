#include "display.hpp"

Display800x600::Display800x600() {
    current_fid = 0;
    frame_big[0] = {};
    frame_big[1] = {};
    frame_big[2] = {};
    screen[0] = {};
    screen[1] = {};
    screen[2] = {};
}

Display800x600::Display800x600(int h_multiple, int v_multiple, void** frame_ptrs) {
	for (int i = 0; i < NBUF; i++) {
	    frame_big[i] = ImageView::fromMemory(frame_ptrs[i], HRES * h_multiple, VRES * v_multiple);
	    screen[i] = frame_big[i].slice(0, 0, HRES, VRES);
	}
    current_fid = 0;
}

void Display800x600::registerSwapCallBack(DisplayCB cb) {
    this->swap_callback = cb;
}
void Display800x600::registerPointCallBack(DisplayCB cb) {
    this->point_callback = cb;
}
void Display800x600::registerReloadCallBack(DisplayCB cb) {
    this->point_callback = cb;
}

void Display800x600::swapBuffers(int fid) {
	if (fid == -1) {
		fid = (current_fid + 1) % NBUF;
	}
    current_fid = fid;
    // call a callback function to do platform specific operations
    if (swap_callback.func) {
        swap_callback.func(this, swap_callback.payload, nullptr);
    }
}

int Display800x600::getCurrentFID() const {
    return current_fid;
}

int Display800x600::getNextFID() const {
    return (current_fid + 1) % NBUF;
}
int Display800x600::getLastFID() const {
    return (current_fid + NBUF - 1) % NBUF;
}

ImageView Display800x600::getFrame(int fid) const {
    return frame_big[fid];
}
ImageView& Display800x600::getFrameRef(int fid) {
    return frame_big[fid];
}

Draw Display800x600::getFrameDraw(int fid) const {
    return Draw::from(getFrame(fid));
}

ImageSlice Display800x600::getScreen(int fid) const {
    return screen[fid];
}

ImageSlice& Display800x600::getScreenRef(int fid) {
    return screen[fid];
}

Draw Display800x600::getScreenDraw(int fid) const {
    return Draw::from(getScreen(fid));
}


void Display800x600::pointScreen(int fid, int x, int y) {
    screen[fid] = frame_big[fid].slice(x, y, HRES, VRES);
    if (point_callback.func) {
        std::pair<int, int> pair = {x, y};
        point_callback.func(this, point_callback.payload, &pair);
    }
}


int Display800x600::reloadFrame(int fid, ImageView view, int x, int y) {
    if (view.width % HRES != 0 || view.height % VRES != 0) {
        return -1;
    }
    frame_big[fid] = view;
    screen[fid] = frame_big[fid].slice(0, 0, HRES, VRES);
    if (reload_callback.func) {
        reload_callback.func(this, point_callback.payload, nullptr);
    }
    screen[fid] = frame_big[fid].slice(x, y, HRES, VRES);
    if (point_callback.func) {
        std::pair<int, int> pair = {x, y};
        point_callback.func(this, point_callback.payload, &pair);
    }
    return 0;
}

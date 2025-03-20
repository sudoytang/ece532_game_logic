#pragma once

#ifndef DBFFER_HPP
#define DBFFER_HPP


#include "../image/image.hpp"
#include "../draw/draw.hpp"
class Display800x600;

struct DisplayCB {
    // function pointer for display callback.
    // parameters:
    //    1. the Display instance
    //    2. the payload from user, user must guarantee memory safety
    //    3. the payload from Display, memory safety is guaranteed by display
    //       but only when the operation is being done. Users CANNOT save it
    //       for future dereference

    typedef void (*FuncPtr)(Display800x600*, void*, void*);
    FuncPtr func;
    void* payload;
    DisplayCB() : func(nullptr), payload(nullptr) {}
};

class Display800x600 {
public:
    static constexpr int HRES = 800;
    static constexpr int VRES = 600;
    static constexpr int NBUF = 3;
private:
    ImageView frame_big[NBUF];
    ImageSlice screen[NBUF];
    int current_fid;
    DisplayCB swap_callback;
    DisplayCB point_callback;
    DisplayCB reload_callback;

public:
    Display800x600();
    Display800x600(int h_multiple, int v_multiple, void** frame_ptrs);
    void registerSwapCallBack(DisplayCB cb);
    void registerPointCallBack(DisplayCB cb);
    void registerReloadCallBack(DisplayCB cb);
    void swapBuffers(int fid = -1);
    int getCurrentFID() const;
    int getNextFID() const;
    int getLastFID() const;
    ImageView getFrame(int fid) const;
    ImageView& getFrameRef(int fid);
    Draw getFrameDraw(int fid) const;
    ImageSlice getScreen(int fid) const;
    ImageSlice& getScreenRef(int fid);
    Draw getScreenDraw(int fid) const;

    void pointScreen(int fid, int x, int y);
    int reloadFrame(int fid, ImageView view, int x = 0, int y = 0);
};




#endif  // DBFFER_HPP

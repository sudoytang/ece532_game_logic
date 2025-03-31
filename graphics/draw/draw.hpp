#pragma once

#ifndef DRAW_HPP
#define DRAW_HPP


#include "../color/color.hpp"
#include "../image/image.hpp"
#include "../font/font.hpp"

#ifdef __MICROBLAZE__
#include "xil_cache.h"
#endif

// class Draw
// sometimes we need to draw something on a specific area of the frame buffer
// so we need to specify the start position, the width & height of the area
// and the buffer to draw on.
class Draw {
private:
    ImageSlice slice;

public:
    Draw() = default;
    Draw(ImageSlice slice);
    ImageSlice getSlice() const;
    void setBlank(Color color = C_TRANSPARENT);
    void drawPixelUnchecked(int x, int y, Color color);
    void drawPixel(int x, int y, Color color);
    void drawPixelAlpha(int x, int y, Color color);
    void drawLineUnchecked(int x0, int y0, int x1, int y1, Color color);
    void drawLine(int x0, int y0, int x1, int y1, Color color);
    void drawLineAlpha(int x0, int y0, int x1, int y1, Color color);
    void drawRectUnchecked(int x0, int y0, int width, int height, Color color, bool fill = false);
    void drawRect(int x0, int y0, int width, int height, Color color, bool fill = false);
    void drawRectAlpha(int x0, int y0, int width, int height, Color color, bool fill = false);
    void drawCircleUnchecked(int xc, int yc, int radius, Color color, bool fill = false);
    void drawCircle(int xc, int yc, int radius, Color color, bool fill = false);
    void drawCircleAlpha(int xc, int yc, int radius, Color color, bool fill = false);
    void drawImageUnchecked(int x, int y, const Image& image);
    void drawImage(int x, int y, const Image& image);
    void drawImageAlpha(int x, int y, const Image& image);
    void drawImageViewUnchecked(int x, int y, ImageView view);
    void drawImageView(int x, int y, ImageView view);
    void drawImageViewAlpha(int x, int y, ImageView view);
    void drawImageSliceUnchecked(int x, int y, ImageSlice sl);
    void drawImageSlice(int x, int y, ImageSlice sl, Color default_color = C_TRANSPARENT);
    void drawImageSliceAlpha(int x, int y, ImageSlice sl, Color default_color = C_TRANSPARENT);
    static Image genTextImage(const Font& font, const char* text, Color color);
    void drawTextUnchecked(const Font& font, int x, int y, const char* text, Color color);
    void drawText(const Font& font, int x, int y, const char* text, Color color);
    void drawTextAlpha(const Font& font, int x, int y, const char* text, Color color);

    static Color alphaComposite(Color bg, Color fg);
    static Draw from(const Image& image);
    static Draw from(ImageView view);
    static Draw from(ImageSlice slice);

#ifdef __MICROBLAZE__
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

	static void FlushPixels(ImageSlice refresh_slice) {
        auto start_cache_ptr = (UINTPTR)getFirstPixelAddress(refresh_slice);
        int cache_len = (UINTPTR)getEndPixelAddress(refresh_slice) - start_cache_ptr;
        Xil_DCacheFlushRange(start_cache_ptr, cache_len);
	}
#else
	static void FlushPixels(ImageSlice refresh_slice) {
		// not on microblaze, don't do anythin
	}
#endif

};



#endif  // DRAW_HPP

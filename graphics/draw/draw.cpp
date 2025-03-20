#include "draw.hpp"
#include <cstring>

Draw::Draw(ImageSlice slice) : slice(slice) {}

ImageSlice Draw::getSlice() const {
    return slice;
}

void Draw::setBlank(Color color) {
	slice.setBlank(color);
}

void Draw::drawPixelUnchecked(int x, int y, Color color) {
	y += slice.y;
	x += slice.x;
	slice.view.data[y * slice.view.width + x] = color;
}

void Draw::drawPixel(int x, int y, Color color) {
	 slice.set(x, y, color);
}

void Draw::drawPixelAlpha(int x, int y, Color color) {
	Color bg = slice.get(x, y, C_TRANSPARENT);
	Color composite = alphaComposite(bg, color);
	slice.set(x, y, composite);
}

void Draw::drawLineUnchecked(int x0, int y0, int x1, int y1, Color color) {
    int dx = x1 - x0;
    int dy = y1 - y0;
    int sx = x0 < x1 ? 1 : -1;
    int sy = y0 < y1 ? 1 : -1;
    int err = dx - dy;
    for (;;) {
        drawPixelUnchecked(x0, y0, color);
        if (x0 == x1 && y0 == y1) {
            break;
        }
        int e2 = err * 2;
        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
    return;
}

void Draw::drawLine(int x0, int y0, int x1, int y1, Color color) {
    int dx = x1 - x0;
    int dy = y1 - y0;
    int sx = x0 < x1 ? 1 : -1;
    int sy = y0 < y1 ? 1 : -1;
    int err = dx - dy;
    if (x0 >= 0 && x0 < slice.width && x0 + slice.x >= 0 && x0 + slice.x < slice.view.width
     && x1 >= 0 && x1 < slice.width && x1 + slice.x >= 0 && x1 + slice.x < slice.view.width
	 && y0 >= 0 && y0 < slice.height && y0 + slice.y >= 0 && y0 + slice.y < slice.view.height
	 && y1 >= 0 && y1 < slice.height && y0 + slice.y >= 0 && y1 + slice.y < slice.view.height) {
    	// perfect, nothing is out of bound
    	return drawLineUnchecked(x0, y0, x1, y1, color);
    }
    for (;;) {
        drawPixel(x0, y0, color);
        if (x0 == x1 && y0 == y1) {
            break;
        }
        int e2 = err * 2;
        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}

void Draw::drawLineAlpha(int x0, int y0, int x1, int y1, Color color) {
    int dx = x1 - x0;
    int dy = y1 - y0;
    int sx = x0 < x1 ? 1 : -1;
    int sy = y0 < y1 ? 1 : -1;
    int err = dx - dy;
    for (;;) {
        drawPixelAlpha(x0, y0, color);
        if (x0 == x1 && y0 == y1) {
            break;
        }
        int e2 = err * 2;
        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}


void Draw::drawRectUnchecked(int x0, int y0, int width, int height, Color color, bool fill) {
    if (fill) {
    	for (int r = 0; r < height; r++) {
    		auto dst = (slice.view.data + (y0 + r + slice.y) * slice.view.width + (x0 + slice.x));
    		std::fill_n(dst, width, color);
    	}
    } else {
    	drawLineUnchecked(x0, y0, x0 + width, y0, color);
    	drawLineUnchecked(x0, y0, x0, y0 + height, color);
    	drawLineUnchecked(x0 + width, y0, x0 + width, y0 + height, color);
    	drawLineUnchecked(x0, y0 + height, x0 + width, y0 + height, color);
    }
}

void Draw::drawRect(int x0, int y0, int width, int height, Color color, bool fill) {
	int x1 = x0 + width;
	int y1 = y0 + height;
	bool check =
		x0 >= 0 && x0 < slice.width && x0 + slice.x >= 0 && x0 + slice.x < slice.view.width
	 && x1 >= 0 && x1 < slice.width && x1 + slice.x >= 0 && x1 + slice.x < slice.view.width
	 && y0 >= 0 && y0 < slice.height && y0 + slice.y >= 0 && y0 + slice.y < slice.view.height
	 && y1 >= 0 && y1 < slice.height && y0 + slice.y >= 0 && y1 + slice.y < slice.view.height;
	if (check) {
		return drawRectUnchecked(x0, y0, width, height, color, fill);
	}
    if (fill) {
        for (int x = x0; x < x0 + width; x++) {
            for (int y = y0; y < y0 + height; y++) {
                drawPixel(x, y, color);
            }
        }
    } else {
        drawLine(x0, y0, x0 + width, y0, color);
        drawLine(x0, y0, x0, y0 + height, color);
        drawLine(x0 + width, y0, x0 + width, y0 + height, color);
        drawLine(x0, y0 + height, x0 + width, y0 + height, color);
    }
}

void Draw::drawRectAlpha(int x0, int y0, int width, int height, Color color, bool fill) {
    if (fill) {
        for (int x = x0; x < x0 + width; x++) {
            for (int y = y0; y < y0 + height; y++) {
                drawPixelAlpha(x, y, color);
            }
        }
    } else {
    	drawLineAlpha(x0, y0, x0 + width, y0, color);
    	drawLineAlpha(x0, y0, x0, y0 + height, color);
    	drawLineAlpha(x0 + width, y0, x0 + width, y0 + height, color);
    	drawLineAlpha(x0, y0 + height, x0 + width, y0 + height, color);
    }
}

void Draw::drawCircleUnchecked(int xc, int yc, int radius, Color color, bool fill) {
    auto plot8points = [this, xc, yc, fill](int x, int y, Color color) {
        if (fill) {
        	drawLineUnchecked(xc - x, yc + y, xc + x, yc + y, color);
        	drawLineUnchecked(xc - x, yc - y, xc + x, yc - y, color);
        	drawLineUnchecked(xc - y, yc + x, xc + y, yc + x, color);
        	drawLineUnchecked(xc - y, yc - x, xc + y, yc - x, color);
        } else {
        	drawPixelUnchecked(xc + x, yc + y, color);
        	drawPixelUnchecked(xc - x, yc + y, color);
        	drawPixelUnchecked(xc + x, yc - y, color);
        	drawPixelUnchecked(xc - x, yc - y, color);
        	drawPixelUnchecked(xc + y, yc + x, color);
        	drawPixelUnchecked(xc - y, yc + x, color);
        	drawPixelUnchecked(xc + y, yc - x, color);
        	drawPixelUnchecked(xc - y, yc - x, color);
        }
    };

    int x = 0;
    int y = radius;
    int d = 3 - 2 * radius;

    plot8points(x, y, color);
    while (y >= x) {
        x++;
        if (d <= 0) {
            d += 4 * x + 6;
        } else {
            y--;
            d += 4 * (x - y) + 10;
        }
        plot8points(x, y, color);
    }
}

void Draw::drawCircle(int xc, int yc, int radius, Color color, bool fill) {
	int x0 = xc - radius;
	int y0 = yc - radius;
	int x1 = xc + radius;
	int y1 = yc + radius;
	bool checked =
		x0 >= 0 && x0 < slice.width && x0 + slice.x >= 0 && x0 + slice.x < slice.view.width
	 && x1 >= 0 && x1 < slice.width && x1 + slice.x >= 0 && x1 + slice.x < slice.view.width
	 && y0 >= 0 && y0 < slice.height && y0 + slice.y >= 0 && y0 + slice.y < slice.view.height
	 && y1 >= 0 && y1 < slice.height && y0 + slice.y >= 0 && y1 + slice.y < slice.view.height;

	if (checked) {
		return drawCircleUnchecked(xc, yc, radius, color, fill);
	}

    auto plot8points = [this, xc, yc, fill](int x, int y, Color color) {
        if (fill) {
            drawLine(xc - x, yc + y, xc + x, yc + y, color);
            drawLine(xc - x, yc - y, xc + x, yc - y, color);
            drawLine(xc - y, yc + x, xc + y, yc + x, color);
            drawLine(xc - y, yc - x, xc + y, yc - x, color);
        } else {
            drawPixel(xc + x, yc + y, color);
            drawPixel(xc - x, yc + y, color);
            drawPixel(xc + x, yc - y, color);
            drawPixel(xc - x, yc - y, color);
            drawPixel(xc + y, yc + x, color);
            drawPixel(xc - y, yc + x, color);
            drawPixel(xc + y, yc - x, color);
            drawPixel(xc - y, yc - x, color);
        }
    };

    int x = 0;
    int y = radius;
    int d = 3 - 2 * radius;

    plot8points(x, y, color);
    while (y >= x) {
        x++;
        if (d <= 0) {
            d += 4 * x + 6;
        } else {
            y--;
            d += 4 * (x - y) + 10;
        }
        plot8points(x, y, color);
    }
}

void Draw::drawCircleAlpha(int xc, int yc, int radius, Color color, bool fill) {
    // Bresenham's algorithm

    auto plot8points = [this, xc, yc, fill](int x, int y, Color color) {
        if (fill) {
            drawLineAlpha(xc - x, yc + y, xc + x, yc + y, color);
            drawLineAlpha(xc - x, yc - y, xc + x, yc - y, color);
            drawLineAlpha(xc - y, yc + x, xc + y, yc + x, color);
            drawLineAlpha(xc - y, yc - x, xc + y, yc - x, color);
        } else {
            drawPixelAlpha(xc + x, yc + y, color);
            drawPixelAlpha(xc - x, yc + y, color);
            drawPixelAlpha(xc + x, yc - y, color);
            drawPixelAlpha(xc - x, yc - y, color);
            drawPixelAlpha(xc + y, yc + x, color);
            drawPixelAlpha(xc - y, yc + x, color);
            drawPixelAlpha(xc + y, yc - x, color);
            drawPixelAlpha(xc - y, yc - x, color);
        }
    };

    int x = 0;
    int y = radius;
    int d = 3 - 2 * radius;

    plot8points(x, y, color);
    while (y >= x) {
        x++;
        if (d <= 0) {
            d += 4 * x + 6;
        } else {
            y--;
            d += 4 * (x - y) + 10;
        }
        plot8points(x, y, color);
    }
}

void Draw::drawImageUnchecked(int x, int y, const Image& image) {
	drawImageViewUnchecked(x, y, image.view());
}

void Draw::drawImage(int x, int y, const Image& image) {
	int x0 = x, y0 = y, x1 = x + image.width, y1 = y + image.height;
	bool checked =
		x0 >= 0 && x0 < slice.width && x0 + slice.x >= 0 && x0 + slice.x < slice.view.width
	 && x1 >= 0 && x1 < slice.width && x1 + slice.x >= 0 && x1 + slice.x < slice.view.width
	 && y0 >= 0 && y0 < slice.height && y0 + slice.y >= 0 && y0 + slice.y < slice.view.height
	 && y1 >= 0 && y1 < slice.height && y0 + slice.y >= 0 && y1 + slice.y < slice.view.height;
	if (checked) {
		return drawImageUnchecked(x, y, image);
	}
    drawImageView(x, y, image.view());
}

void Draw::drawImageAlpha(int x, int y, const Image& image) {
    drawImageViewAlpha(x, y, image.view());
}

void Draw::drawImageViewUnchecked(int x, int y, ImageView view) {
	for (int r = 0; r < view.height; r++) {
		auto src = (char*)(view.data + r * view.width);
		auto dst = (char*)(slice.view.data + (y + r + slice.y) * slice.view.width + (x + slice.x));
		memcpy(dst, src, view.width * sizeof(Color));
	}
}

void Draw::drawImageView(int x, int y, ImageView view) {
	auto x0 = x, y0 = y, x1 = x0 + view.width, y1 = y0 + view.height;
	bool checked =
		x0 >= 0 && x0 < slice.width && x0 + slice.x >= 0 && x0 + slice.x < slice.view.width
	 && x1 >= 0 && x1 < slice.width && x1 + slice.x >= 0 && x1 + slice.x < slice.view.width
	 && y0 >= 0 && y0 < slice.height && y0 + slice.y >= 0 && y0 + slice.y < slice.view.height
	 && y1 >= 0 && y1 < slice.height && y0 + slice.y >= 0 && y1 + slice.y < slice.view.height;
	if (checked) {
		return drawImageViewUnchecked(x, y, view);
	}
	drawImageSlice(x, y, view.slice());
}

void Draw::drawImageViewAlpha(int x, int y, ImageView view) {
	drawImageSliceAlpha(x, y, view.slice());
}


void Draw::drawImageSliceUnchecked(int x, int y, ImageSlice sl) {
	for (int r = 0; r < sl.height; r++) {
		auto src = (char*)(sl.view.data + (r + sl.y) * sl.view.width + sl.x);
		auto dst = (char*)(slice.view.data + (y + r + slice.y) * slice.view.width + (x + slice.x));
		memcpy(dst, src, sl.width * sizeof(Color));
	}
}


void Draw::drawImageSlice(int x, int y, ImageSlice sl, Color default_color) {
	auto x0 = x, y0 = y, x1 = x0 + sl.width, y1 = y0 + sl.height;
	auto x2 = sl.x, y2 = sl.y, x3 = sl.x + sl.width, y3 = sl.y + sl.height;
	bool checked =
		x0 >= 0 && x0 < slice.width && x0 + slice.x >= 0 && x0 + slice.x < slice.view.width
	 && x1 >= 0 && x1 < slice.width && x1 + slice.x >= 0 && x1 + slice.x < slice.view.width
	 && y0 >= 0 && y0 < slice.height && y0 + slice.y >= 0 && y0 + slice.y < slice.view.height
	 && y1 >= 0 && y1 < slice.height && y0 + slice.y >= 0 && y1 + slice.y < slice.view.height;

	bool checked2 =
			x2 >= 0 && x2 < sl.view.width
		 && x3 >= 0 && x3 < sl.view.width
		 && y2 >= 0 && y2 < sl.view.height
		 && y3 >= 0 && y3 < sl.view.height;
	if (checked && checked2) {
		return drawImageSliceUnchecked(x, y, sl);
	}
    for (int j = 0; j < sl.height; j++) {
        for (int i = 0; i < sl.width; i++) {
            drawPixel(x + i, y + j, sl.get(i, j, default_color));
        }
    }
}
void Draw::drawImageSliceAlpha(int x, int y, ImageSlice slice, Color default_color) {
    for (int j = 0; j < slice.height; j++) {
        for (int i = 0; i < slice.width; i++) {
            drawPixelAlpha(x + i, y + j, slice.get(i, j, default_color));
        }
    }
}

Image Draw::genTextImage(const Font& font, const char* text, Color color) {
	// WARNING: This assumes the font is monospaced!
	// NEVER USE non-monospaced fonts
    Image img = Image::fromBlank(font.getGlyph('A').width * strlen(text), font.getGlyph('A').height, C_TRANSPARENT);
    Draw draw(img.slice(0, 0, img.width, img.height));
    draw.drawTextUnchecked(font, 0, 0, text, color);
    return img;
}

void Draw::drawTextUnchecked(const Font& font, int x, int y, const char* text, Color color) {
    for (int i = 0; text[i] != '\0'; i++) {
        ImageSlice current_char_slice = font.getGlyph(text[i]);
        drawImageSliceUnchecked(x, y, current_char_slice);
        x += current_char_slice.width;
    }
}


void Draw::drawText(const Font& font, int x, int y, const char* text, Color color) {
    for (int i = 0; text[i] != '\0'; i++) {
        ImageSlice current_char_slice = font.getGlyph(text[i]);
        drawImageSlice(x, y, current_char_slice, color);
        x += current_char_slice.width;
    }
}

void Draw::drawTextAlpha(const Font& font, int x, int y, const char* text, Color color) {
    for (int i = 0; text[i] != '\0'; i++) {
        ImageSlice current_char_slice = font.getGlyph(text[i]);
        drawImageSliceAlpha(x, y, current_char_slice, color);
        x += current_char_slice.width;
    }
}

Color Draw::alphaComposite(Color bg, Color fg) {
    if (fg.a) {
        return fg;
    } else if (bg.a) {
        return bg;
    } else {
        return C_TRANSPARENT;
    }
}

Draw Draw::from(const Image& image) {
    return Draw(image.slice());
}

Draw Draw::from(ImageView view) {
    return Draw(view.slice());
}

Draw Draw::from(ImageSlice slice) {
    return Draw(slice);
}


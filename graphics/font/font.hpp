#pragma once

#ifndef FONT_HPP
#define FONT_HPP

#include <array>
#include "../image/image.hpp"

#include "../resource/font_map.hpp"

struct BBox {
    int x;
    int y;
    int width;
    int height;
};

struct Glyph {
    char charcode;
    BBox bbox;
};

class Font {
private:
    static constexpr char START_PRINTABLE = 32;
    static constexpr char END_PRINTABLE = 127;
    static constexpr int GLYPH_COUNT = END_PRINTABLE - START_PRINTABLE;
    static constexpr int GetFontImageHeight(int char_per_row, int glyph_height) {
        int n_char_rows = GLYPH_COUNT / char_per_row + (GLYPH_COUNT % char_per_row > 0);
        return n_char_rows * glyph_height;
    }
    std::array<Glyph, END_PRINTABLE - START_PRINTABLE> glyphs;
    Image font_image;

public:
    Font();
    ImageSlice getGlyph(char c) const;
    bool isValidFont() const;
    static Font fromImageView(ImageView view, int glyph_width, int glyph_height, int char_per_row);
    static Font DEFAULT_FONT;
    static void initDefaultFont();

};



#endif  // FONT_HPP
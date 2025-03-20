#include "font.hpp"

Font::Font() {
    glyphs = {};
}

ImageSlice Font::getGlyph(char c) const {
    auto bbox = glyphs[c - START_PRINTABLE].bbox;
    return font_image.slice(bbox.x, bbox.y, bbox.width, bbox.height);
}

bool Font::isValidFont() const {
    return font_image.data;
}

Font Font::fromImageView(ImageView view, int glyph_width, int glyph_height, int char_per_row) {
    Font font;
    font.font_image = view.toImage();
    for (char c = START_PRINTABLE; c < END_PRINTABLE; c++) {
        int index = c - START_PRINTABLE;
        int col = index % char_per_row;
        int row = index / char_per_row;
        int x = col * glyph_width;
        int y = row * glyph_height;
        Glyph glyph;
        glyph.charcode = c;
        glyph.bbox = BBox {x, y, glyph_width, glyph_height};
        font.glyphs[index] = glyph;
    }
    return font;
}

Font Font::DEFAULT_FONT;

void Font::initDefaultFont() {
    DEFAULT_FONT = Font::fromImageView(
        ImageView::fromMemory((void*)font_map, 16 * 8, GetFontImageHeight(8, 16)),
        16, 16, 8
    );
}
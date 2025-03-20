#pragma once

#ifndef COLOR_HPP
#define COLOR_HPP

#include <cstdint>

struct Color {
    union {
        struct {
            uint16_t r : 4;
            uint16_t g : 4;
            uint16_t b : 4;
            uint16_t a : 4;
        };
        uint16_t value;
    };

    Color(uint16_t value): value(value) {}
    Color() : value(0) {}

    bool operator==(const Color& other) const {
        return value == other.value;
    }

    bool operator!=(const Color& other) const {
        return value != other.value;
    }
};

extern const Color C_BLACK;
extern const Color C_WHITE;
extern const Color C_RED;
extern const Color C_GREEN;
extern const Color C_BLUE;
extern const Color C_YELLOW;
extern const Color C_CYAN;
extern const Color C_MAGENTA;
extern const Color C_GRAY;
extern const Color C_TRANSPARENT;


#endif // COLOR_HPP

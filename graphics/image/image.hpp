#pragma once

#ifndef IMAGE_HPP
#define IMAGE_HPP


#include "../color/color.hpp"
#include "../../memory/memory.hpp"

struct Image;
struct ImageView;
struct ImageSlice;



struct Image {
    int width;
    int height;
    ManagedArray<Color> data;

    Image();
    Image(const Image& other) = delete;
    Image& operator=(const Image& other) = delete;

    Image(Image&& other) noexcept;
    Image& operator=(Image&& other) noexcept;

    Image clone() const;
    ImageView view() const;
    ImageSlice slice(int x, int y, int width, int height) const;
    ImageSlice slice() const;

    Color get(int x, int y, Color default_color = C_TRANSPARENT) const;
    bool set(int x, int y, Color color);
    void swap(Image& other);

    void setBlank(Color color = C_TRANSPARENT);

    static Image fromMemory(void* data, int width, int height);
    static Image fromBlank(int width, int height, Color color = C_TRANSPARENT);
};

struct ImageView {
    int width;
    int height;
    Color* data;

    Image toImage() const;
    Image upscale(int times) const;
    Image downscale(int times) const;
    ImageSlice slice(int x, int y, int width, int height) const;
    ImageSlice slice() const;
    bool set(int x, int y, Color color);
    Color get(int x, int y, Color default_color = C_TRANSPARENT) const;

    void setBlank(Color color = C_TRANSPARENT);

    static ImageView fromMemory(void* data, int width, int height);
};

struct ImageSlice {
    int width;
    int height;
    int x;
    int y;
    ImageView view;

    Image toImage(Color default_color = C_TRANSPARENT) const;
    ImageSlice slice(int x, int y, int width, int height) const;
    ImageView getView() const;

    Color get(int x, int y, Color default_color = C_TRANSPARENT) const;
    bool set(int x, int y, Color color);

    void setBlank(Color color = C_TRANSPARENT);
    static ImageSlice fromBlank(int width, int height);
};

#endif

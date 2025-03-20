#include "image.hpp"
#include <cstring>
#include <algorithm>

Image::Image(): width(0), height(0), data(nullptr) {}

Image::Image(Image&& other) noexcept {
    width = other.width;
    height = other.height;
    data = std::move(other.data);
    other.width = 0;
    other.height = 0;
}

Image& Image::operator=(Image&& other) noexcept {
    width = other.width;
    height = other.height;
    data = std::move(other.data);
    other.width = 0;
    other.height = 0;
    return *this;
}

Image Image::clone() const {
    Image clone;
    clone.width = width;
    clone.height = height;
    clone.data = makeManagedVramArray<Color>(width * height);
    std::memcpy(clone.data.get(), data.get(), width * height * sizeof(Color));
    return clone;
}

ImageView Image::view() const {
    return ImageView::fromMemory(data.get(), width, height);
}

ImageSlice Image::slice(int x, int y, int width, int height) const {
    return view().slice(x, y, width, height);
}

ImageSlice Image::slice() const {
    return slice(0, 0, width, height);
}

Color Image::get(int x, int y, Color default_color) const {
    if (x < 0 || x >= width || y < 0 || y >= height) {
        return default_color;
    }
    return data[y * width + x];
}

bool Image::set(int x, int y, Color color) {
    if (x < 0 || x >= width || y < 0 || y >= height) {
        return false;
    }
    data[y * width + x] = color;
    return true;
}

void Image::setBlank(Color color) {
    std::fill_n(data.get(), width * height, color);
}

void Image::swap(Image& other) {
    std::swap(width, other.width);
    std::swap(height, other.height);
    data.swap(other.data);
}

Image Image::fromMemory(void* data, int width, int height) {
    Image image;
    image.width = width;
    image.height = height;
    image.data = makeManagedVramArray<Color>(width * height);
    std::memcpy(image.data.get(), data, width * height * sizeof(Color));
    return image;
}

Image Image::fromBlank(int width, int height, Color color) {
    Image image;
    image.width = width;
    image.height = height;
    image.data = makeManagedVramArray<Color>(width * height);
    std::fill_n(image.data.get(), width * height, color);
    return image;
}

Image ImageView::toImage() const {
    return Image::fromMemory(data, width, height);
}

ImageSlice ImageView::slice(int x, int y, int width, int height) const {
    ImageSlice slice;
    slice.width = width;
    slice.height = height;
    slice.x = x;
    slice.y = y;
    slice.view = *this;
    return slice;
}

ImageSlice ImageView::slice() const {
    return slice(0, 0, width, height);
}

Color ImageView::get(int x, int y, Color default_color) const {
    if (x < 0 || x >= width || y < 0 || y >= height) {
        return default_color;
    }
    return data[y * width + x];
}

bool ImageView::set(int x, int y, Color color) {
    if (x < 0 || x >= width || y < 0 || y >= height) {
        return false;
    }
    data[y * width + x] = color;
    return true;
}

void ImageView::setBlank(Color color) {
	std::fill_n(data, width * height, color);
}

ImageView ImageView::fromMemory(void* data, int width, int height) {
    ImageView view;
    view.width = width;
    view.height = height;
    view.data = static_cast<Color*>(data);
    return view;
}

// methods for ImageSlice is a little bit tricky
// because it is a slice into an image
// so we need to be careful about the boundaries

Image ImageSlice::toImage(Color default_color) const {
    Image image = Image::fromBlank(width, height, default_color);

    for (int slice_y = y, image_y = 0; slice_y < y + height; slice_y++, image_y++) {
        for (int slice_x = x, image_x = 0; slice_x < x + width; slice_x++, image_x++) {
            if (slice_x < 0 || slice_x >= view.width || slice_y < 0 || slice_y >= view.height) {
                image.data[image_y * width + image_x] = default_color;
            } else {
                image.data[image_y * width + image_x] = view.data[slice_y * view.width + slice_x];
            }
        }
    }
    return image;
}

// ImageSlice::slice
// WARNING: this method may not give you the slice actually you want
// because if you slice a slice and the second slice is out of bounds of the
// first slice, some pixels not in the first slice will be included in the
// second slice (because they are in the view) so you may need to handle the
// boundaries yourself.

ImageSlice ImageSlice::slice(int x, int y, int width, int height) const {
    return view.slice(x + this->x, y + this->y, width, height);
}

ImageView ImageSlice::getView() const {
    return view;
}

Color ImageSlice::get(int x, int y, Color default_color) const {
    int view_x = this->x + x;
    int view_y = this->y + y;
    if (x < 0 || x >= width || y < 0 || y >= height) {
        return default_color;
    }
    return view.get(view_x, view_y);
}

bool ImageSlice::set(int x, int y, Color color) {
    int view_x = this->x + x;
    int view_y = this->y + y;
    if (x < 0 || x >= width || y < 0 || y >= height) {
        return false;
    }
    return view.set(view_x, view_y, color);
}

void ImageSlice::setBlank(Color color) {
	for (int r = 0; r < height; r++) {
		auto dst = (view.data + (r + y) * view.width + x);
		std::fill_n(dst, std::max(width, view.width - x), color);
	}
}

// ImageSlice::fromBlank
// we create a view with width and height 0
// so that everything is out of bounds
// when you want to make it an image, you can set the default color
ImageSlice ImageSlice::fromBlank(int width, int height) {
    ImageView view;
    view.width = 0;
    view.height = 0;
    view.data = nullptr;
    ImageSlice slice;
    slice.width = width;
    slice.height = height;
    slice.x = 0;
    slice.y = 0;
    slice.view = view;
    return slice;
}

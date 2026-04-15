#include "image/Image.h"

#include <cstddef>
#include <limits>
#include <stdexcept>

Image::Image(std::size_t width, std::size_t height) : width_(width), height_(height) {
    if (width == 0 || height == 0) {
        throw std::invalid_argument("Image width and height must be non-zero");
    }

    const std::size_t size_t_max_limit = std::numeric_limits<std::size_t>::max();
    if (width > size_t_max_limit / height) {
        throw std::overflow_error("Image size overflow");
    }

    pixels_.resize(width * height);
}

std::size_t Image::GetWidth() const {
    return width_;
}

std::size_t Image::GetHeight() const {
    return height_;
}

std::size_t Image::Index(std::size_t x, std::size_t y) const {
    return x + width_ * y;
}

Color& Image::At(std::size_t x, std::size_t y) {
    if (x >= width_ || y >= height_) {
        throw std::out_of_range("Index out of range");
    }

    std::size_t index = Index(x, y);
    return pixels_[index];
}

const Color& Image::At(std::size_t x, std::size_t y) const {
    if (x >= width_ || y >= height_) {
        throw std::out_of_range("Index out of range");
    }

    std::size_t index = Index(x, y);
    return pixels_[index];
}

const std::vector<Color>& Image::Pixels() const {
    return pixels_;
}

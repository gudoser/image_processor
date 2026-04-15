#pragma once

#include <cstddef>
#include <vector>

#include "image/Color.h"

class Image {
public:
    Image() = default;
    Image(std::size_t width, std::size_t height);

    std::size_t GetWidth() const;
    std::size_t GetHeight() const;

    Color& At(std::size_t x, std::size_t y);
    const Color& At(std::size_t x, std::size_t y) const;

    const std::vector<Color>& Pixels() const;

private:
    std::size_t Index(std::size_t x, std::size_t y) const;

    std::size_t width_ = 0;
    std::size_t height_ = 0;
    std::vector<Color> pixels_{};
};

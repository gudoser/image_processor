#include "CropFilter.h"

#include <algorithm>
#include <cstddef>
#include <stdexcept>
#include <utility>

#include "image/Image.h"

CropFilter::CropFilter(std::size_t width, std::size_t height) : width_(width), height_(height) {
    if (width == 0 || height == 0) {
        throw std::invalid_argument("Incorrect width or height");
    }
}

void CropFilter::Apply(Image& image) const {
    const std::size_t cropped_width = std::min(width_, image.GetWidth());
    const std::size_t cropped_height = std::min(height_, image.GetHeight());
    Image cropped(cropped_width, cropped_height);

    for (std::size_t y = 0; y < cropped_height; ++y) {
        for (std::size_t x = 0; x < cropped_width; ++x) {
            cropped.At(x, y) = image.At(x, y);
        }
    }

    image = std::move(cropped);
}

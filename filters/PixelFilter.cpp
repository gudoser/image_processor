#include "PixelFilter.h"

#include <cstddef>
#include <utility>

void PixelFilter::Apply(Image& image) const {
    Image result(image.GetWidth(), image.GetHeight());

    for (std::size_t y = 0; y < result.GetHeight(); ++y) {
        for (std::size_t x = 0; x < result.GetWidth(); ++x) {
            result.At(x, y) = ApplyToPixel(image.At(x, y));
        }
    }

    image = std::move(result);
}

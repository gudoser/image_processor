#include "MatrixFilter.h"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <utility>
#include <vector>

namespace {

void ValidateKernel(const std::vector<std::vector<double>>& kernel) {
    if (kernel.empty() || kernel.front().empty()) {
        throw std::invalid_argument("Filter kernel must not be empty");
    }

    const std::size_t kernel_width = kernel.front().size();
    if (kernel.size() % 2 == 0 || kernel_width % 2 == 0) {
        throw std::invalid_argument("Filter kernel dimensions must be odd");
    }

    for (const std::vector<double>& row : kernel) {
        if (row.size() != kernel_width) {
            throw std::invalid_argument("Filter kernel rows must have equal size");
        }
    }
}

std::size_t ClampCoordinate(std::int64_t coordinate, std::size_t limit) {
    if (coordinate < 0) {
        return 0;
    }

    const std::size_t normalized_coordinate = static_cast<std::size_t>(coordinate);
    if (normalized_coordinate >= limit) {
        return limit - 1;
    }

    return normalized_coordinate;
}

unsigned char ClampColorComponent(double value) {
    const double max_color_value = 255.0;
    value = std::clamp(std::round(value), 0.0, max_color_value);
    return static_cast<unsigned char>(value);
}

Color ApplyKernelToPixel(const Image& image, std::size_t x, std::size_t y,
                         const std::vector<std::vector<double>>& kernel) {
    const std::size_t kernel_height = kernel.size();
    const std::size_t kernel_width = kernel.front().size();
    const std::size_t center_y = kernel_height / 2;
    const std::size_t center_x = kernel_width / 2;

    double red = 0.0;
    double green = 0.0;
    double blue = 0.0;

    for (std::size_t kernel_y = 0; kernel_y < kernel_height; ++kernel_y) {
        for (std::size_t kernel_x = 0; kernel_x < kernel_width; ++kernel_x) {
            const std::int64_t source_x = static_cast<std::int64_t>(x) + static_cast<std::int64_t>(kernel_x) -
                                          static_cast<std::int64_t>(center_x);
            const std::int64_t source_y = static_cast<std::int64_t>(y) + static_cast<std::int64_t>(kernel_y) -
                                          static_cast<std::int64_t>(center_y);
            const Color& source_color =
                image.At(ClampCoordinate(source_x, image.GetWidth()), ClampCoordinate(source_y, image.GetHeight()));
            const double weight = kernel[kernel_y][kernel_x];

            red += weight * source_color.GetRed();
            green += weight * source_color.GetGreen();
            blue += weight * source_color.GetBlue();
        }
    }

    return Color(ClampColorComponent(red), ClampColorComponent(green), ClampColorComponent(blue));
}

}  // namespace

void MatrixFilter::Apply(Image& image) const {
    const std::vector<std::vector<double>>& kernel = GetKernel();
    ValidateKernel(kernel);

    Image result(image.GetWidth(), image.GetHeight());
    for (std::size_t y = 0; y < result.GetHeight(); ++y) {
        for (std::size_t x = 0; x < result.GetWidth(); ++x) {
            result.At(x, y) = ApplyKernelToPixel(image, x, y, kernel);
        }
    }

    image = std::move(result);
}

#include "BlurFilter.h"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <vector>

namespace {

struct PixelComponents {
    double red = 0.0;
    double green = 0.0;
    double blue = 0.0;
};

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

std::size_t CalculateRadius(double sigma) {
    if (!std::isfinite(sigma) || sigma <= 0.0) {
        throw std::invalid_argument("Blur sigma must be positive");
    }

    const std::size_t radius = std::max<std::size_t>(1, static_cast<std::size_t>(std::ceil(3.0 * sigma)));
    if (radius > (std::numeric_limits<std::size_t>::max() - 1) / 2) {
        throw std::out_of_range("Blur sigma is too large");
    }

    return radius;
}

std::vector<double> BuildGaussianKernel(double sigma, std::size_t radius) {
    const std::size_t kernel_size = radius * 2 + 1;
    std::vector<double> kernel(kernel_size, 0.0);

    const double sigma_squared = sigma * sigma;
    const double denominator = 2.0 * sigma_squared;
    double weight_sum = 0.0;

    for (std::size_t index = 0; index < kernel_size; ++index) {
        const double delta = static_cast<double>(index) - static_cast<double>(radius);
        const double weight = std::exp(-(delta * delta) / denominator);

        kernel[index] = weight;
        weight_sum += weight;
    }

    if (weight_sum == 0.0) {
        throw std::runtime_error("Failed to build blur kernel");
    }

    for (double& weight : kernel) {
        weight /= weight_sum;
    }

    return kernel;
}

}  // namespace

BlurFilter::BlurFilter(double sigma) : radius_(CalculateRadius(sigma)), kernel_(BuildGaussianKernel(sigma, radius_)) {
}

void BlurFilter::Apply(Image& image) const {
    const std::size_t width = image.GetWidth();
    const std::size_t height = image.GetHeight();

    std::vector<PixelComponents> horizontal_pass(width * height);
    for (std::size_t y = 0; y < height; ++y) {
        for (std::size_t x = 0; x < width; ++x) {
            double red = 0.0;
            double green = 0.0;
            double blue = 0.0;

            for (std::size_t kernel_index = 0; kernel_index < kernel_.size(); ++kernel_index) {
                const std::int64_t source_x = static_cast<std::int64_t>(x) + static_cast<std::int64_t>(kernel_index) -
                                              static_cast<std::int64_t>(radius_);
                const Color& source_color = image.At(ClampCoordinate(source_x, width), y);
                const double weight = kernel_[kernel_index];

                red += weight * source_color.GetRed();
                green += weight * source_color.GetGreen();
                blue += weight * source_color.GetBlue();
            }

            horizontal_pass[y * width + x] = PixelComponents{red, green, blue};
        }
    }

    Image result(width, height);
    for (std::size_t y = 0; y < height; ++y) {
        for (std::size_t x = 0; x < width; ++x) {
            double red = 0.0;
            double green = 0.0;
            double blue = 0.0;

            for (std::size_t kernel_index = 0; kernel_index < kernel_.size(); ++kernel_index) {
                const std::int64_t source_y = static_cast<std::int64_t>(y) + static_cast<std::int64_t>(kernel_index) -
                                              static_cast<std::int64_t>(radius_);
                const PixelComponents& source_pixel = horizontal_pass[ClampCoordinate(source_y, height) * width + x];
                const double weight = kernel_[kernel_index];

                red += weight * source_pixel.red;
                green += weight * source_pixel.green;
                blue += weight * source_pixel.blue;
            }

            result.At(x, y) = Color(ClampColorComponent(red), ClampColorComponent(green), ClampColorComponent(blue));
        }
    }

    image = std::move(result);
}

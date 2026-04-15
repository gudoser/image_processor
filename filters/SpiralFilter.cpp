#include "filters/SpiralFilter.h"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <utility>

namespace {

constexpr double MaxColorValue = 255.0;
constexpr double SpiralStrength = 5.0;

double ClampDouble(double value, double left, double right) {
    return std::clamp(value, left, right);
}

unsigned char ClampColorComponent(double value) {
    return static_cast<unsigned char>(ClampDouble(std::round(value), 0.0, MaxColorValue));
}

double Lerp(double left, double right, double position) {
    return left + (right - left) * position;
}

Color BilinearSample(const Image& image, double x, double y) {
    const double max_x = static_cast<double>(image.GetWidth() - 1);
    const double max_y = static_cast<double>(image.GetHeight() - 1);

    x = ClampDouble(x, 0.0, max_x);
    y = ClampDouble(y, 0.0, max_y);

    const std::size_t left_x = static_cast<std::size_t>(std::floor(x));
    const std::size_t top_y = static_cast<std::size_t>(std::floor(y));
    const std::size_t right_x = std::min(left_x + 1, image.GetWidth() - 1);
    const std::size_t bottom_y = std::min(top_y + 1, image.GetHeight() - 1);

    const double horizontal_position = x - static_cast<double>(left_x);
    const double vertical_position = y - static_cast<double>(top_y);

    const Color& top_left = image.At(left_x, top_y);
    const Color& top_right = image.At(right_x, top_y);
    const Color& bottom_left = image.At(left_x, bottom_y);
    const Color& bottom_right = image.At(right_x, bottom_y);

    const double top_red = Lerp(top_left.GetRed(), top_right.GetRed(), horizontal_position);
    const double top_green = Lerp(top_left.GetGreen(), top_right.GetGreen(), horizontal_position);
    const double top_blue = Lerp(top_left.GetBlue(), top_right.GetBlue(), horizontal_position);

    const double bottom_red = Lerp(bottom_left.GetRed(), bottom_right.GetRed(), horizontal_position);
    const double bottom_green = Lerp(bottom_left.GetGreen(), bottom_right.GetGreen(), horizontal_position);
    const double bottom_blue = Lerp(bottom_left.GetBlue(), bottom_right.GetBlue(), horizontal_position);

    return Color(ClampColorComponent(Lerp(top_red, bottom_red, vertical_position)),
                 ClampColorComponent(Lerp(top_green, bottom_green, vertical_position)),
                 ClampColorComponent(Lerp(top_blue, bottom_blue, vertical_position)));
}

double CalculateMaxRadius(const Image& image, double center_x, double center_y) {
    const double max_dx = std::max(center_x, static_cast<double>(image.GetWidth() - 1) - center_x);
    const double max_dy = std::max(center_y, static_cast<double>(image.GetHeight() - 1) - center_y);
    return std::sqrt(max_dx * max_dx + max_dy * max_dy);
}

std::pair<double, double> MapToSourceCoordinates(std::size_t x, std::size_t y, double center_x, double center_y,
                                                 double max_radius) {
    const double dx = static_cast<double>(x) - center_x;
    const double dy = static_cast<double>(y) - center_y;
    const double radius = std::sqrt(dx * dx + dy * dy);

    if (radius == 0.0 || max_radius == 0.0) {
        return {static_cast<double>(x), static_cast<double>(y)};
    }

    const double normalized_radius = ClampDouble(radius / max_radius, 0.0, 1.0);
    const double spiral_angle = SpiralStrength * (1.0 - normalized_radius) * (1.0 - normalized_radius);
    const double source_angle = std::atan2(dy, dx) - spiral_angle;

    return {
        center_x + radius * std::cos(source_angle),
        center_y + radius * std::sin(source_angle),
    };
}

}  // namespace

void SpiralFilter::Apply(Image& image) const {
    Image result(image.GetWidth(), image.GetHeight());

    const double center_x = static_cast<double>(image.GetWidth() - 1) / 2.0;
    const double center_y = static_cast<double>(image.GetHeight() - 1) / 2.0;
    const double max_radius = CalculateMaxRadius(image, center_x, center_y);

    for (std::size_t y = 0; y < image.GetHeight(); ++y) {
        for (std::size_t x = 0; x < image.GetWidth(); ++x) {
            const auto [source_x, source_y] = MapToSourceCoordinates(x, y, center_x, center_y, max_radius);
            result.At(x, y) = BilinearSample(image, source_x, source_y);
        }
    }

    image = std::move(result);
}

#include "EdgeFilter.h"

#include <cstddef>
#include <stdexcept>
#include <vector>

#include "filters/GrayscaleFilter.h"
#include "filters/MatrixFilter.h"

namespace {

class EdgeMatrixFilter : public MatrixFilter {
protected:
    const std::vector<std::vector<double>>& GetKernel() const override {
        static const std::vector<std::vector<double>> KERNEL = {
            {0.0, -1.0, 0.0},
            {-1.0, 4.0, -1.0},
            {0.0, -1.0, 0.0},
        };
        return KERNEL;
    }
};

}  // namespace

EdgeFilter::EdgeFilter(double threshold) : threshold_(threshold) {
    if (threshold < 0.0 || threshold > 1.0) {
        throw std::invalid_argument("Edge threshold must be in range [0, 1]");
    }
}

void EdgeFilter::Apply(Image& image) const {
    const unsigned char white = 255;
    const unsigned char black = 0;
    const double max_color_value = 255.0;
    const double threshold_value = threshold_ * max_color_value;

    GrayscaleFilter grayscale_filter;
    grayscale_filter.Apply(image);

    EdgeMatrixFilter edge_matrix_filter;
    edge_matrix_filter.Apply(image);

    for (std::size_t y = 0; y < image.GetHeight(); ++y) {
        for (std::size_t x = 0; x < image.GetWidth(); ++x) {
            if (image.At(x, y).GetRed() > threshold_value) {
                const unsigned char color_value = white;
                image.At(x, y) = Color(color_value, color_value, color_value);
            } else {
                const unsigned char color_value = black;
                image.At(x, y) = Color(color_value, color_value, color_value);
            }
        }
    }
}

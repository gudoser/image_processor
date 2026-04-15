#include "SharpenFilter.h"

#include <vector>

const std::vector<std::vector<double>>& SharpenFilter::GetKernel() const {
    static const std::vector<std::vector<double>> KERNEL = {
        {0.0, -1.0, 0.0},
        {-1.0, 5.0, -1.0},
        {0.0, -1.0, 0.0},
    };

    return KERNEL;
}

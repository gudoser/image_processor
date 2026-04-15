#pragma once

#include <cstddef>
#include <vector>

#include "filters/IFilter.h"

class BlurFilter : public IFilter {
public:
    explicit BlurFilter(double sigma);
    void Apply(Image& image) const override;

private:
    std::size_t radius_ = 0;
    std::vector<double> kernel_;
};

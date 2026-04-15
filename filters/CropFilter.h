#pragma once

#include <cstddef>

#include "IFilter.h"

class CropFilter : public IFilter {
public:
    CropFilter(std::size_t width, std::size_t height);

    void Apply(Image& image) const override;

private:
    std::size_t width_;
    std::size_t height_;
};

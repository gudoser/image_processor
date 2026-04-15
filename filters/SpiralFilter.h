#pragma once

#include "filters/IFilter.h"

class SpiralFilter : public IFilter {
public:
    void Apply(Image& image) const override;
};

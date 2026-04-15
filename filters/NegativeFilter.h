#pragma once

#include "PixelFilter.h"

class NegativeFilter : public PixelFilter {
public:
    Color ApplyToPixel(const Color& color) const override;
};

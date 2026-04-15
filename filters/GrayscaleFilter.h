#pragma once

#include "PixelFilter.h"

class GrayscaleFilter : public PixelFilter {
public:
    Color ApplyToPixel(const Color& color) const override;
};

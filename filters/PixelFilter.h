#pragma once

#include "filters/IFilter.h"
#include "image/Color.h"

class PixelFilter : public IFilter {
public:
    void Apply(Image& image) const override;

protected:
    virtual Color ApplyToPixel(const Color& color) const = 0;
};

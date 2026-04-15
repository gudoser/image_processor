#pragma once

#include "filters/IFilter.h"

class EdgeFilter : public IFilter {
public:
    explicit EdgeFilter(double threshold);

    void Apply(Image& image) const override;

private:
    double threshold_;
};

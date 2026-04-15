#pragma once

#include <vector>

#include "filters/IFilter.h"

class MatrixFilter : public IFilter {
public:
    void Apply(Image& image) const override;

protected:
    virtual const std::vector<std::vector<double>>& GetKernel() const = 0;
};

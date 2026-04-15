#pragma once

#include <vector>

#include "filters/MatrixFilter.h"

class SharpenFilter : public MatrixFilter {
protected:
    const std::vector<std::vector<double>>& GetKernel() const override;
};

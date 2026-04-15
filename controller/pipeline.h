#pragma once

#include <memory>
#include <vector>

#include "filters/IFilter.h"
#include "image/Image.h"

class Pipeline {
public:
    Pipeline() = default;
    ~Pipeline() = default;

    void Add(std::unique_ptr<IFilter> filter);
    void Apply(Image& image) const;

private:
    std::vector<std::unique_ptr<IFilter>> filters_;
};

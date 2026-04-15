#include "pipeline.h"

#include <cstddef>

#include "image/Image.h"

void Pipeline::Add(std::unique_ptr<IFilter> filter) {
    filters_.push_back(std::move(filter));
}

void Pipeline::Apply(Image& image) const {
    for (const std::unique_ptr<IFilter>& filter : filters_) {
        filter->Apply(image);
    }
}

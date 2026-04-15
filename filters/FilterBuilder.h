#pragma once

#include <memory>

#include "controller/parser.h"
#include "filters/IFilter.h"

std::unique_ptr<IFilter> BuildFilter(const FilterSpec& spec);

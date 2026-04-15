#include "filters/FilterBuilder.h"

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <memory>
#include <stdexcept>
#include <string>

#include "filters/BlurFilter.h"
#include "filters/CropFilter.h"
#include "filters/EdgeFilter.h"
#include "filters/GrayscaleFilter.h"
#include "filters/NegativeFilter.h"
#include "filters/SharpenFilter.h"
#include "filters/SpiralFilter.h"

namespace {

std::size_t ParsePositiveSize(const std::string& value, const std::string& argument_name) {
    if (!value.empty() && value.front() == '-') {
        throw std::invalid_argument("Filter argument '" + argument_name + "' must be a positive integer");
    }

    std::size_t parsed_characters = 0;
    std::uint64_t parsed_value = 0;

    try {
        parsed_value = static_cast<std::uint64_t>(std::stoull(value, &parsed_characters));
    } catch (const std::exception&) {
        throw std::invalid_argument("Filter argument '" + argument_name + "' must be a positive integer");
    }

    if (parsed_characters != value.size() || parsed_value == 0) {
        throw std::invalid_argument("Filter argument '" + argument_name + "' must be a positive integer");
    }

    if (parsed_value > std::numeric_limits<std::size_t>::max()) {
        throw std::out_of_range("Filter argument '" + argument_name + "' is too large");
    }

    return static_cast<std::size_t>(parsed_value);
}

double ParseDoubleValue(const std::string& value, const std::string& argument_name) {
    std::size_t parsed_characters = 0;
    double parsed_value = 0.0;

    try {
        parsed_value = std::stod(value, &parsed_characters);
    } catch (const std::exception&) {
        throw std::invalid_argument("Filter argument '" + argument_name + "' must be a number");
    }

    if (parsed_characters != value.size() || !std::isfinite(parsed_value)) {
        throw std::invalid_argument("Filter argument '" + argument_name + "' must be a valid finite number");
    }

    return parsed_value;
}

}  // namespace

std::unique_ptr<IFilter> BuildFilter(const FilterSpec& spec) {
    if (spec.name == "crop") {
        const std::size_t width = ParsePositiveSize(spec.arguments.at(0), "width");
        const std::size_t height = ParsePositiveSize(spec.arguments.at(1), "height");
        return std::make_unique<CropFilter>(width, height);
    }

    if (spec.name == "gs") {
        return std::make_unique<GrayscaleFilter>();
    }

    if (spec.name == "neg") {
        return std::make_unique<NegativeFilter>();
    }

    if (spec.name == "sharp") {
        return std::make_unique<SharpenFilter>();
    }

    if (spec.name == "edge") {
        const double threshold = ParseDoubleValue(spec.arguments.at(0), "threshold");
        return std::make_unique<EdgeFilter>(threshold);
    }

    if (spec.name == "blur") {
        const double sigma = ParseDoubleValue(spec.arguments.at(0), "sigma");
        return std::make_unique<BlurFilter>(sigma);
    }

    if (spec.name == "spiral") {
        return std::make_unique<SpiralFilter>();
    }

    throw std::invalid_argument("Filter is not implemented: -" + spec.name);
}

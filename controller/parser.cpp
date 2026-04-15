#include "parser.h"

#include <cstddef>
#include <stdexcept>
#include <string>

namespace {

std::size_t GetArgumentCount(const std::string& filter_name) {
    if (filter_name == "crop") {
        return 2;
    }
    if (filter_name == "gs" || filter_name == "neg" || filter_name == "sharp") {
        return 0;
    }
    if (filter_name == "edge" || filter_name == "blur") {
        return 1;
    }
    if (filter_name == "spiral") {
        return 0;
    }

    throw std::invalid_argument("Unknown filter: -" + filter_name);
}

void ValidateFilterArguments(const FilterSpec& filter) {
    const std::size_t expected_argument_count = GetArgumentCount(filter.name);
    if (filter.arguments.size() != expected_argument_count) {
        throw std::invalid_argument("Filter -" + filter.name + " expects " + std::to_string(expected_argument_count) +
                                    " argument(s)");
    }
}

}  // namespace

CommandLineOptions ParseCommandLine(int argc, char** argv) {
    if (argc < 3) {
        throw std::invalid_argument("Not enough arguments");
    }

    CommandLineOptions command_line_options;
    command_line_options.input_path = argv[1];
    command_line_options.output_path = argv[2];

    for (int i = 3; i < argc; ++i) {
        if (argv[i][0] != '-') {
            throw std::invalid_argument("Expected filter name starting with '-'");
        }

        FilterSpec filter;
        filter.name = argv[i] + 1;
        const std::size_t expected_argument_count = GetArgumentCount(filter.name);

        for (std::size_t argument_index = 0; argument_index < expected_argument_count; ++argument_index) {
            ++i;
            if (i >= argc) {
                ValidateFilterArguments(filter);
            }

            filter.arguments.emplace_back(argv[i]);
        }

        ValidateFilterArguments(filter);
        command_line_options.filter_specs.push_back(filter);
    }

    return command_line_options;
}

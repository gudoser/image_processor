#pragma once

#include <string>
#include <vector>

struct FilterSpec {
    std::string name;
    std::vector<std::string> arguments;
};

struct CommandLineOptions {
    std::string input_path;
    std::string output_path;
    std::vector<FilterSpec> filter_specs;
};

CommandLineOptions ParseCommandLine(int argc, char** argv);

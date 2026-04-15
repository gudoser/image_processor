#include <exception>
#include <iostream>
#include <string_view>

#include "bmp/BmpReader.h"
#include "bmp/BmpWriter.h"
#include "controller/help.h"
#include "controller/parser.h"
#include "controller/pipeline.h"
#include "filters/FilterBuilder.h"

int main(int argc, char** argv) {
    if (argc == 1) {
        PrintHelp(std::cout);
        return 0;
    }

    for (int i = 1; i < argc; ++i) {
        if (!IsHelpArg(argv[i])) {
            continue;
        }

        for (int filter_index = i - 1; filter_index >= 1; --filter_index) {
            std::string_view candidate = argv[filter_index];
            if (IsHelpArg(candidate)) {
                continue;
            }
            if (IsFilterName(candidate)) {
                PrintFilterHelp(std::cout, candidate);
                return 0;
            }
            if (!candidate.empty() && candidate.front() == '-') {
                std::cerr << "Error: unknown filter '" << candidate << "'\n";
                return 1;
            }
        }

        PrintHelp(std::cout);
        return 0;
    }

    if (argc < 3) {
        std::cerr << "Error: not enough arguments\n";
        PrintHelp(std::cerr);
        return 1;
    }

    try {
        CommandLineOptions options = ParseCommandLine(argc, argv);
        Image image = ReadBmp(options.input_path);

        Pipeline pipeline;
        for (const FilterSpec& filter_spec : options.filter_specs) {
            pipeline.Add(BuildFilter(filter_spec));
        }
        pipeline.Apply(image);

        WriteBmp(options.output_path, image);
        return 0;
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << '\n';
        return 1;
    } catch (...) {
        std::cerr << "Error: unknown exception\n";
        return 1;
    }
}

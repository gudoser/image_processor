#include "help.h"

#include <string>

void PrintHelp(std::ostream& out) {
    out << "Usage:\n";
    out << "  image_processor <input.bmp> <output.bmp> [filters...]\n\n";
    out << "Arguments:\n";
    out << "  <input.bmp>    path to source 24-bit BMP image\n";
    out << "  <output.bmp>   path to output BMP image\n\n";
    out << "Filters are applied from left to right in the order they are listed.\n";
    out << "Available filters:\n";
    out << "  -crop <width> <height>\n";
    out << "      Crop image to the top-left area of given size.\n";
    out << "  -gs\n";
    out << "      Convert image to grayscale.\n";
    out << "  -neg\n";
    out << "      Convert image to negative.\n";
    out << "  -sharp\n";
    out << "      Apply sharpening filter.\n";
    out << "  -edge <threshold>\n";
    out << "      Detect edges using the specified threshold.\n";
    out << "  -blur <sigma>\n";
    out << "      Apply gaussian blur with the specified sigma.\n\n";
    out << "  -spiral\n";
    out << "      Twist the image smoothly around its center.\n\n";
    out << "Help:\n";
    out << "  -h, -help, --help\n";
    out << "      Show this message.\n";
    out << "  -<filter> -h\n";
    out << "      Show help for a specific filter.\n";
}

std::string NormalizeFilterName(std::string_view filter_arg) {
    if (!filter_arg.empty() && filter_arg.front() == '-') {
        filter_arg.remove_prefix(1);
    }
    return std::string(filter_arg);
}

bool IsFilterName(std::string_view filter_name) {
    const std::string normalized_name = NormalizeFilterName(filter_name);
    return normalized_name == "crop" || normalized_name == "gs" || normalized_name == "neg" ||
           normalized_name == "sharp" || normalized_name == "edge" || normalized_name == "blur" ||
           normalized_name == "spiral";
}

bool PrintFilterHelp(std::ostream& out, std::string_view filter_name) {
    const std::string normalized_name = NormalizeFilterName(filter_name);

    if (normalized_name == "crop") {
        out << "Filter: -crop <width> <height>\n";
        out << "Crop the image to the top-left area of the specified size.\n";
        return true;
    }

    if (normalized_name == "gs") {
        out << "Filter: -gs\n";
        out << "Convert the image to grayscale.\n";
        return true;
    }

    if (normalized_name == "neg") {
        out << "Filter: -neg\n";
        out << "Convert the image to negative.\n";
        return true;
    }

    if (normalized_name == "sharp") {
        out << "Filter: -sharp\n";
        out << "Apply the sharpening convolution filter.\n";
        return true;
    }

    if (normalized_name == "edge") {
        out << "Filter: -edge <threshold>\n";
        out << "Detect edges after grayscale conversion using the specified threshold.\n";
        return true;
    }

    if (normalized_name == "blur") {
        out << "Filter: -blur <sigma>\n";
        out << "Apply gaussian blur with the specified sigma.\n";
        return true;
    }

    if (normalized_name == "spiral") {
        out << "Filter: -spiral\n";
        out << "Twist the image smoothly around its center.\n";
        return true;
    }

    return false;
}

bool IsHelpArg(std::string_view arg) {
    return arg == "-help" || arg == "--help" || arg == "-h";
}

#include <catch.hpp>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <initializer_list>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "../bmp/BmpReader.h"
#include "../bmp/BmpWriter.h"
#include "../controller/help.h"
#include "../controller/parser.h"
#include "../filters/FilterBuilder.h"
#include "../image/Image.h"

namespace {

std::vector<char*> ToArgv(std::vector<std::string>& args) {
    std::vector<char*> argv;
    argv.reserve(args.size());
    for (std::string& arg : args) {
        argv.push_back(arg.data());
    }
    return argv;
}

CommandLineOptions Parse(std::initializer_list<std::string> raw_args) {
    std::vector<std::string> args(raw_args);
    std::vector<char*> argv = ToArgv(args);
    return ParseCommandLine(static_cast<int>(argv.size()), argv.data());
}

class ScopedTemporaryPath {
public:
    explicit ScopedTemporaryPath(std::filesystem::path path) : path_(std::move(path)) {
    }

    ~ScopedTemporaryPath() {
        std::error_code error_code;
        std::filesystem::remove(path_, error_code);
    }

    const std::filesystem::path& Get() const {
        return path_;
    }

private:
    std::filesystem::path path_;
};

std::filesystem::path MakeTemporaryPath(const std::string& stem) {
    const auto unique_id = std::chrono::steady_clock::now().time_since_epoch().count();
    return std::filesystem::temp_directory_path() / (stem + "_" + std::to_string(unique_id) + ".bmp");
}

}  // namespace

TEST_CASE("Image reports invalid size and invalid coordinates") {
    REQUIRE_THROWS_AS(Image(0, 1), std::invalid_argument);
    REQUIRE_THROWS_AS(Image(1, 0), std::invalid_argument);

    Image image(1, 1);
    REQUIRE_THROWS_AS(image.At(1, 0), std::out_of_range);
    REQUIRE_THROWS_AS(image.At(0, 1), std::out_of_range);
}

TEST_CASE("ParseCommandLine rejects invalid command line") {
    REQUIRE_THROWS_AS(Parse({"image_processor", "in.bmp"}), std::invalid_argument);
    REQUIRE_THROWS_AS(Parse({"image_processor", "in.bmp", "out.bmp", "-unknown"}), std::invalid_argument);
    REQUIRE_THROWS_AS(Parse({"image_processor", "in.bmp", "out.bmp", "-crop", "5"}), std::invalid_argument);
    REQUIRE_THROWS_AS(Parse({"image_processor", "in.bmp", "out.bmp", "crop"}), std::invalid_argument);
}

TEST_CASE("ParseCommandLine keeps negative numbers as filter arguments") {
    const CommandLineOptions options =
        Parse({"image_processor", "in.bmp", "out.bmp", "-blur", "-1", "-edge", "-0.1"});

    REQUIRE(options.filter_specs.size() == 2);
    CHECK(options.filter_specs[0].name == "blur");
    CHECK(options.filter_specs[0].arguments == std::vector<std::string>{"-1"});
    CHECK(options.filter_specs[1].name == "edge");
    CHECK(options.filter_specs[1].arguments == std::vector<std::string>{"-0.1"});
}

TEST_CASE("Help helper functions recognize supported arguments") {
    CHECK(IsHelpArg("-h"));
    CHECK(IsHelpArg("-help"));
    CHECK(IsHelpArg("--help"));
    CHECK_FALSE(IsHelpArg("-gs"));

    CHECK(IsFilterName("crop"));
    CHECK(IsFilterName("-blur"));
    CHECK(IsFilterName("-spiral"));
    CHECK_FALSE(IsFilterName("-unknown"));

    std::ostringstream filter_help_output;
    CHECK(PrintFilterHelp(filter_help_output, "-edge"));
    CHECK(filter_help_output.str().find("Filter: -edge <threshold>") != std::string::npos);

    std::ostringstream unknown_help_output;
    CHECK_FALSE(PrintFilterHelp(unknown_help_output, "-unknown"));
}

TEST_CASE("BuildFilter rejects invalid filter arguments") {
    REQUIRE_THROWS_AS(BuildFilter(FilterSpec{"crop", {"0", "2"}}), std::invalid_argument);
    REQUIRE_THROWS_AS(BuildFilter(FilterSpec{"crop", {"-1", "2"}}), std::invalid_argument);
    REQUIRE_THROWS_AS(BuildFilter(FilterSpec{"blur", {"0"}}), std::invalid_argument);
    REQUIRE_THROWS_AS(BuildFilter(FilterSpec{"blur", {"inf"}}), std::invalid_argument);
    REQUIRE_THROWS_AS(BuildFilter(FilterSpec{"blur", {"nan"}}), std::invalid_argument);
    REQUIRE_THROWS_AS(BuildFilter(FilterSpec{"edge", {"-0.1"}}), std::invalid_argument);
    REQUIRE_THROWS_AS(BuildFilter(FilterSpec{"edge", {"1.1"}}), std::invalid_argument);
    REQUIRE_THROWS_AS(BuildFilter(FilterSpec{"edge", {"inf"}}), std::invalid_argument);
    REQUIRE_THROWS_AS(BuildFilter(FilterSpec{"unknown", {}}), std::invalid_argument);
}

TEST_CASE("BuildFilter creates supported filters") {
    CHECK_NOTHROW(BuildFilter(FilterSpec{"crop", {"3", "2"}}));
    CHECK_NOTHROW(BuildFilter(FilterSpec{"gs", {}}));
    CHECK_NOTHROW(BuildFilter(FilterSpec{"neg", {}}));
    CHECK_NOTHROW(BuildFilter(FilterSpec{"sharp", {}}));
    CHECK_NOTHROW(BuildFilter(FilterSpec{"edge", {"0.4"}}));
    CHECK_NOTHROW(BuildFilter(FilterSpec{"blur", {"1.0"}}));
    CHECK_NOTHROW(BuildFilter(FilterSpec{"spiral", {}}));
}

TEST_CASE("BmpReader and BmpWriter reject invalid input") {
    ScopedTemporaryPath invalid_bmp_path(MakeTemporaryPath("image_processor_invalid"));
    {
        std::ofstream output(invalid_bmp_path.Get());
        output << "not a bmp";
    }

    REQUIRE_THROWS_AS(ReadBmp(invalid_bmp_path.Get().string()), std::runtime_error);

    const Image empty_image;
    const ScopedTemporaryPath empty_output_path(MakeTemporaryPath("image_processor_empty"));
    REQUIRE_THROWS_AS(WriteBmp(empty_output_path.Get().string(), empty_image), std::invalid_argument);
}

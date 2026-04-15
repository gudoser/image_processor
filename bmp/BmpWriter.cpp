#include "BmpWriter.h"

#include "BmpReader.h"
#include "image/Color.h"

#include <array>
#include <cstdint>
#include <fstream>
#include <limits>
#include <stdexcept>
#include <string>

namespace {

void ValidateBmpDimensions(std::size_t width, std::size_t height) {
    if (width == 0 || height == 0) {
        throw std::invalid_argument("Image width and height must be non-zero");
    }

    const std::uint64_t max_int32 = std::numeric_limits<std::int32_t>::max();
    const std::uint64_t width64 = width;
    const std::uint64_t height64 = height;

    if (width64 > max_int32 || height64 > max_int32) {
        throw std::overflow_error("Image dimensions are too large for BMP");
    }

    const std::uint64_t row_size_without_padding = width64 * 3;
    const std::uint64_t padding = (4 - (row_size_without_padding % 4)) % 4;
    const std::uint64_t row_size = row_size_without_padding + padding;
    const std::uint64_t pixel_data_size = row_size * height64;
    const std::uint64_t file_size = 14 + 40 + pixel_data_size;

    if (row_size > std::numeric_limits<std::uint32_t>::max() ||
        pixel_data_size > std::numeric_limits<std::uint32_t>::max() ||
        file_size > std::numeric_limits<std::uint32_t>::max()) {
        throw std::overflow_error("BMP file is too large");
    }
}

std::ofstream OpenBmpOutput(const std::string& path) {
    std::ofstream output(path, std::ios::binary);
    if (!output.is_open()) {
        throw std::runtime_error("Failed to open output BMP file: " + path);
    }
    return output;
}

template <typename T>
void WriteValue(std::ofstream& output, T value) {
    output.write(reinterpret_cast<const char*>(&value), sizeof(T));
    if (!output) {
        throw std::runtime_error("Failed to write BMP value");
    }
}

std::uint32_t CalculateRowPadding(std::size_t width) {
    const std::uint64_t row_size = static_cast<std::uint64_t>(width) * 3;
    return static_cast<std::uint32_t>((4 - (row_size % 4)) % 4);
}

FileHeader MakeFileHeader(std::size_t width, std::size_t height) {
    const std::uint16_t header_type = 0x4D42;
    const std::uint32_t header_size = 14 + 40;
    const std::uint32_t padding = CalculateRowPadding(width);
    const std::uint32_t row_size = static_cast<std::uint32_t>(static_cast<std::uint64_t>(width) * 3) + padding;
    const std::uint32_t pixel_data_size = row_size * static_cast<std::uint32_t>(height);

    FileHeader header{};
    header.type = header_type;
    header.file_size = header_size + pixel_data_size;
    header.reserved1 = 0;
    header.reserved2 = 0;
    header.pixel_data_offset = header_size;
    return header;
}

InfoHeader MakeInfoHeader(const Image& image) {
    const std::uint32_t header_size = 40;
    const std::uint16_t bits_per_pixel = 24;
    const std::size_t width = image.GetWidth();
    const std::size_t height = image.GetHeight();
    const std::uint32_t padding = CalculateRowPadding(width);
    const std::uint32_t row_size = static_cast<std::uint32_t>(static_cast<std::uint64_t>(width) * 3) + padding;

    InfoHeader header{};
    header.header_size = header_size;
    header.width = static_cast<std::int32_t>(width);
    header.height = static_cast<std::int32_t>(height);
    header.planes = 1;
    header.bits_per_pixel = bits_per_pixel;
    header.compression = 0;
    header.image_size = row_size * static_cast<std::uint32_t>(height);
    header.x_pixels_per_meter = 0;
    header.y_pixels_per_meter = 0;
    header.colors_used = 0;
    header.important_colors = 0;
    return header;
}

void WriteFileHeader(std::ofstream& output, const FileHeader& header) {
    WriteValue(output, header.type);
    WriteValue(output, header.file_size);
    WriteValue(output, header.reserved1);
    WriteValue(output, header.reserved2);
    WriteValue(output, header.pixel_data_offset);
}

void WriteInfoHeader(std::ofstream& output, const InfoHeader& header) {
    WriteValue(output, header.header_size);
    WriteValue(output, header.width);
    WriteValue(output, header.height);
    WriteValue(output, header.planes);
    WriteValue(output, header.bits_per_pixel);
    WriteValue(output, header.compression);
    WriteValue(output, header.image_size);
    WriteValue(output, header.x_pixels_per_meter);
    WriteValue(output, header.y_pixels_per_meter);
    WriteValue(output, header.colors_used);
    WriteValue(output, header.important_colors);
}

void WritePixel(std::ofstream& output, const Color& color) {
    WriteValue(output, color.GetBlue());
    WriteValue(output, color.GetGreen());
    WriteValue(output, color.GetRed());
}

void WritePadding(std::ofstream& output, std::uint32_t padding) {
    const std::array<std::uint8_t, 3> zeros = {0, 0, 0};
    output.write(reinterpret_cast<const char*>(zeros.data()), static_cast<std::streamsize>(padding));
    if (!output) {
        throw std::runtime_error("Failed to write BMP row padding");
    }
}

void WritePixelArray(std::ofstream& output, const Image& image) {
    const std::size_t width = image.GetWidth();
    const std::size_t height = image.GetHeight();
    const std::uint32_t padding = CalculateRowPadding(width);

    for (std::size_t row = height; row > 0; --row) {
        const std::size_t image_row = row - 1;
        for (std::size_t x = 0; x < width; ++x) {
            WritePixel(output, image.At(x, image_row));
        }
        WritePadding(output, padding);
    }
}

}  // namespace

void WriteBmp(const std::string& path, const Image& image) {
    ValidateBmpDimensions(image.GetWidth(), image.GetHeight());

    std::ofstream output = OpenBmpOutput(path);
    const FileHeader file_header = MakeFileHeader(image.GetWidth(), image.GetHeight());
    const InfoHeader info_header = MakeInfoHeader(image);

    WriteFileHeader(output, file_header);
    WriteInfoHeader(output, info_header);
    WritePixelArray(output, image);
}

#include "BmpReader.h"

#include "image/Color.h"
#include "image/Image.h"

#include <cstdint>
#include <fstream>
#include <limits>
#include <stdexcept>
#include <string>

namespace {

std::ifstream OpenBmpInput(const std::string& path) {
    std::ifstream input(path, std::ios::binary);

    if (!input.is_open()) {
        throw std::runtime_error("Failed to open BMP file: " + path);
    }

    return input;
}

template <typename T>
T ReadValue(std::ifstream& input) {
    T value{};
    input.read(reinterpret_cast<char*>(&value), sizeof(T));

    if (!input) {
        throw std::runtime_error("Failed to read value from BMP file");
    }

    return value;
}

FileHeader ReadFileHeader(std::ifstream& input) {
    FileHeader header{};

    header.type = ReadValue<std::uint16_t>(input);
    header.file_size = ReadValue<std::uint32_t>(input);
    header.reserved1 = ReadValue<std::uint16_t>(input);
    header.reserved2 = ReadValue<std::uint16_t>(input);
    header.pixel_data_offset = ReadValue<std::uint32_t>(input);

    return header;
}

InfoHeader ReadInfoHeader(std::ifstream& input) {
    InfoHeader header{};

    header.header_size = ReadValue<std::uint32_t>(input);
    header.width = ReadValue<std::int32_t>(input);
    header.height = ReadValue<std::int32_t>(input);
    header.planes = ReadValue<std::uint16_t>(input);
    header.bits_per_pixel = ReadValue<std::uint16_t>(input);
    header.compression = ReadValue<std::uint32_t>(input);
    header.image_size = ReadValue<std::uint32_t>(input);
    header.x_pixels_per_meter = ReadValue<std::int32_t>(input);
    header.y_pixels_per_meter = ReadValue<std::int32_t>(input);
    header.colors_used = ReadValue<std::uint32_t>(input);
    header.important_colors = ReadValue<std::uint32_t>(input);

    return header;
}

void ValidateFileHeader(const FileHeader& header) {
    const std::uint16_t bmp_signature = 0x4D42;

    if (header.type != bmp_signature) {
        throw std::runtime_error("File is not a BMP");
    }
}

void ValidateInfoHeader(const InfoHeader& header) {
    const std::uint32_t expected_header_size = 40;
    const std::uint16_t expected_bit_per_pixel = 24;

    if (header.header_size != expected_header_size) {
        throw std::runtime_error("Unsupported header: expected BITMAPINFOHEADER");
    }

    if (header.planes != 1) {
        throw std::runtime_error("Invalid BMP: planes must be 1");
    }

    if (header.bits_per_pixel != expected_bit_per_pixel) {
        throw std::runtime_error("Unsupported BMP: only 24-bit BMP is supported");
    }

    if (header.compression != 0) {
        throw std::runtime_error("Unsupported BMP: compressed BMP is not supported");
    }

    if (header.width <= 0) {
        throw std::runtime_error("Invalid BMP: width must be positive");
    }

    if (header.height == 0) {
        throw std::runtime_error("Invalid BMP: height must not be zero");
    }

    if (header.height == std::numeric_limits<std::int32_t>::min()) {
        throw std::runtime_error("Invalid BMP: absolute height is too large");
    }
}

void ValidatePixelDataOffset(const FileHeader& file_header, const InfoHeader& info_header) {
    const std::uint32_t minimum_pixel_data_offset = 14 + info_header.header_size;
    if (file_header.pixel_data_offset < minimum_pixel_data_offset) {
        throw std::runtime_error("Invalid BMP: pixel data overlaps header");
    }
}

std::size_t GetImageHeight(const InfoHeader& info_header) {
    if (info_header.height < 0) {
        return static_cast<std::size_t>(-info_header.height);
    }
    return static_cast<std::size_t>(info_header.height);
}

std::uint32_t CalculateRowPadding(std::size_t width) {
    const std::uint64_t row_bytes = static_cast<std::uint64_t>(width) * 3;
    if (row_bytes > std::numeric_limits<std::uint32_t>::max()) {
        throw std::overflow_error("BMP row is too wide");
    }

    const std::uint32_t padding = (4 - (row_bytes % 4)) % 4;
    return padding;
}

void SeekToPixelArray(std::ifstream& input, const FileHeader& file_header) {
    input.seekg(file_header.pixel_data_offset, std::ios::beg);

    if (!input) {
        throw std::runtime_error("Failed to seek to BMP pixel data");
    }
}

Color ReadPixel(std::ifstream& input) {
    unsigned char b = ReadValue<unsigned char>(input);
    unsigned char g = ReadValue<unsigned char>(input);
    unsigned char r = ReadValue<unsigned char>(input);

    return Color(r, g, b);
}

void SkipPadding(std::ifstream& input, std::uint32_t padding) {
    input.seekg(padding, std::ios::cur);

    if (!input) {
        throw std::runtime_error("Failed to skip BMP row padding");
    }
}

std::size_t GetImageRowIndex(std::size_t file_row, std::size_t height, bool is_bottom_up) {
    if (is_bottom_up) {
        return height - 1 - file_row;
    }
    return file_row;
}

Image ReadPixelArray(std::ifstream& input, const InfoHeader& info_header) {
    const std::size_t width = static_cast<std::size_t>(info_header.width);
    const std::size_t height = GetImageHeight(info_header);
    const bool is_bottom_up = info_header.height > 0;
    const std::uint32_t padding = CalculateRowPadding(width);

    Image image(width, height);
    for (std::size_t row = 0; row < height; ++row) {
        const std::size_t image_row = GetImageRowIndex(row, height, is_bottom_up);
        for (std::size_t x = 0; x < width; ++x) {
            Color color = ReadPixel(input);
            image.At(x, image_row) = color;
        }
        SkipPadding(input, padding);
    }

    return image;
}

}  // namespace

Image ReadBmp(const std::string& path) {
    std::ifstream input = OpenBmpInput(path);
    FileHeader file_header = ReadFileHeader(input);
    InfoHeader info_header = ReadInfoHeader(input);

    ValidateFileHeader(file_header);
    ValidateInfoHeader(info_header);
    ValidatePixelDataOffset(file_header, info_header);

    SeekToPixelArray(input, file_header);

    return ReadPixelArray(input, info_header);
}

#pragma once

#include "image/Image.h"

#include <cstdint>
#include <string>

struct FileHeader {
    std::uint16_t type;
    std::uint32_t file_size;
    std::uint16_t reserved1;
    std::uint16_t reserved2;
    std::uint32_t pixel_data_offset;
};

struct InfoHeader {
    std::uint32_t header_size;
    std::int32_t width;
    std::int32_t height;
    std::uint16_t planes;
    std::uint16_t bits_per_pixel;
    std::uint32_t compression;
    std::uint32_t image_size;
    std::int32_t x_pixels_per_meter;
    std::int32_t y_pixels_per_meter;
    std::uint32_t colors_used;
    std::uint32_t important_colors;
};

Image ReadBmp(const std::string& path);

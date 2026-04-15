cmake_minimum_required(VERSION 3.16)
project(image_processor LANGUAGES CXX)

add_executable(
    image_processor
    image_processor.cpp
    bmp/BmpReader.cpp
    bmp/BmpWriter.cpp
    controller/help.cpp
    controller/parser.cpp
    controller/pipeline.cpp
    image/Color.cpp
    image/Image.cpp
    filters/CropFilter.cpp
    filters/FilterBuilder.cpp
    filters/PixelFilter.cpp
    filters/MatrixFilter.cpp
    filters/BlurFilter.cpp
    filters/EdgeFilter.cpp
    filters/GrayscaleFilter.cpp
    filters/NegativeFilter.cpp
    filters/SharpenFilter.cpp
    filters/SpiralFilter.cpp
)

target_include_directories(image_processor PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(image_processor PRIVATE cxx_std_17)

if (COMMAND add_catch)
    add_catch(
        test_image_processor_unit
        tests/test.cpp
        bmp/BmpReader.cpp
        bmp/BmpWriter.cpp
        controller/help.cpp
        controller/parser.cpp
        controller/pipeline.cpp
        image/Color.cpp
        image/Image.cpp
        filters/CropFilter.cpp
        filters/FilterBuilder.cpp
        filters/PixelFilter.cpp
        filters/MatrixFilter.cpp
        filters/BlurFilter.cpp
        filters/EdgeFilter.cpp
        filters/GrayscaleFilter.cpp
        filters/NegativeFilter.cpp
        filters/SharpenFilter.cpp
        filters/SpiralFilter.cpp
    )
endif()

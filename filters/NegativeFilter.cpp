#include "NegativeFilter.h"

#include "image/Color.h"

Color NegativeFilter::ApplyToPixel(const Color& color) const {
    const unsigned char max_color_value = 255;

    unsigned char new_red = max_color_value - color.GetRed();
    unsigned char new_green = max_color_value - color.GetGreen();
    unsigned char new_blue = max_color_value - color.GetBlue();

    return Color(new_red, new_green, new_blue);
}

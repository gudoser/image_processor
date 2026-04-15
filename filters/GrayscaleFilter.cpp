#include "GrayscaleFilter.h"

#include "image/Color.h"

Color GrayscaleFilter::ApplyToPixel(const Color& color) const {
    const double red_coef = 0.299;
    const double green_coef = 0.587;
    const double blue_coef = 0.114;

    double gray = red_coef * color.GetRed() + green_coef * color.GetGreen() + blue_coef * color.GetBlue();
    unsigned char uc_gray = static_cast<unsigned char>(gray);
    Color gray_color(uc_gray, uc_gray, uc_gray);

    return gray_color;
}

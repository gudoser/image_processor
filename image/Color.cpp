#include "Color.h"

Color::Color(unsigned char red, unsigned char green, unsigned char blue) : red_(red), green_(green), blue_(blue) {
}

unsigned char Color::GetRed() const {
    return red_;
}

unsigned char Color::GetGreen() const {
    return green_;
}

unsigned char Color::GetBlue() const {
    return blue_;
}

void Color::SetRed(unsigned char red) {
    red_ = red;
}

void Color::SetGreen(unsigned char green) {
    green_ = green;
}

void Color::SetBlue(unsigned char blue) {
    blue_ = blue;
}

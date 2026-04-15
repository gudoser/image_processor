#pragma once

class Color {
public:
    Color() = default;
    Color(unsigned char red, unsigned char green, unsigned char blue);

    unsigned char GetRed() const;
    unsigned char GetGreen() const;
    unsigned char GetBlue() const;

    void SetRed(unsigned char red);
    void SetGreen(unsigned char green);
    void SetBlue(unsigned char blue);

private:
    unsigned char red_ = 0;
    unsigned char green_ = 0;
    unsigned char blue_ = 0;
};

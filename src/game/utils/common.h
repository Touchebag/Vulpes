#pragma once

#include <string>

namespace util {

struct Vec2i {
    Vec2i(int x_pos, int y_pos) : x(x_pos), y(y_pos) {}
    Vec2i() = default;

    int x = 0;
    int y = 0;
};

struct Vec2d {
    Vec2d(double x_pos, double y_pos) : x(x_pos), y(y_pos) {}
    Vec2d() = default;

    double x = 0.0;
    double y = 0.0;
};

struct Rectangle {
    Rectangle(int x_pos, int y_pos, int rect_width, int rect_height) :
        x(x_pos),
        y(y_pos),
        width(rect_width),
        height(rect_height) {}

    Rectangle() = default;
    Rectangle(const Rectangle&) = default;
    Rectangle& operator=(const Rectangle&) = default;
    Rectangle(Rectangle&&) = default;
    Rectangle& operator=(Rectangle&&) = default;

    int x;
    int y;
    int width;
    int height;
};

inline bool closeToZero(double value, double threshold) {
    return std::abs(value) < threshold;
}

} // namespace

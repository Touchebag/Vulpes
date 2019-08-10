#pragma once

namespace util {

struct Point {
    Point(int x_pos, int y_pos) : x(x_pos), y(y_pos) {}

    const int x;
    const int y;
};

struct Rectangle {
    Rectangle(int x_pos, int y_pos, int rect_width, int rect_height) :
        x(x_pos),
        y(y_pos),
        width(rect_width),
        height(rect_height) {}

    const int x;
    const int y;
    const int width;
    const int height;
};

} // namespace

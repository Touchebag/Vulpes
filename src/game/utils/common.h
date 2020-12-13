#pragma once

#include <string>

namespace util {

static const std::string MAIN_ENTITY_NAME = "main_entity_name";
static const std::string COMMON_ASSET_DIR = "common";

struct Point {
    Point(int x_pos, int y_pos) : x(x_pos), y(y_pos) {}

    Point() = default;
    Point(const Point&) = default;
    Point& operator=(const Point&) = default;
    Point(Point&&) = default;
    Point& operator=(Point&&) = default;

    int x;
    int y;
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

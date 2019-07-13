#pragma once

namespace util {

class Point {
  public:
    Point(int x, int y) : x_(x), y_(y) {}

    const int x_;
    const int y_;
};

} // namespace

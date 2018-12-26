#pragma once

#include "util.h"

class Transform {
public:
    void setPosition(util::X x, util::Y y);
    void move(util::X x, util::Y y);

    util::X getX();
    util::Y getY();

private:
    util::X pos_x_ = util::X(0.0);
    util::Y pos_y_ = util::Y(0.0);
};

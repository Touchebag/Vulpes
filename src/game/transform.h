#pragma once

#include "utils/common.h"

class Transform {
public:
    void setPosition(int x, int y);

    int getX();
    int getY();

private:
    int pos_x_ = 0;
    int pos_y_ = 0;
};

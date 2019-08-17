#pragma once

#include <json.hpp>

#include "utils/common.h"

class Transform {
public:
    void setPosition(int x, int y);

    void loadFromJson(nlohmann::json j);
    std::optional<nlohmann::json> outputToJson();

    int getX() const;
    int getY() const;

private:
    int pos_x_ = 0;
    int pos_y_ = 0;
};

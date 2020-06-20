#pragma once

#include <json.hpp>

#include "component.h"

#include "utils/common.h"

class Transform : public Component {
public:
    void setPosition(int x, int y);
    void setPosition(util::Point point);
    util::Point getPosition();

    void update() override;

    void reloadFromJson(nlohmann::json j) override;
    std::optional<nlohmann::json> outputToJson() override;

    int getX() const;
    int getY() const;

private:
    int pos_x_ = 0;
    int pos_y_ = 0;
};

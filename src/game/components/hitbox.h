#pragma once

#include <tuple>

#include "component.h"

#include "utils/common.h"
#include "json.hpp"

class Hitbox : public Component {
  public:
    void setHitbox(int width, int height);

    void update() override;

    void reloadFromJson(nlohmann::json j) override;
    std::optional<nlohmann::json> outputToJson() override;

    void setOffset(std::pair<int, int> offset);
    void moveOffset(std::pair<int, int> offset);

    int width_ = 0;
    int height_ = 0;

    int getRight() const;
    int getLeft() const;
    int getTop() const;
    int getBottom() const;

  private:
    std::pair<int, int> offset_ = {0, 0};
};

#pragma once

#include "component.h"

class Death : public Component {
  public:
    void update() override;

    void reloadFromJson(nlohmann::json j) override;
    std::optional<nlohmann::json> outputToJson() override;

    void setDead();

    bool isDead();

  private:
    bool is_dead_ = false;
};

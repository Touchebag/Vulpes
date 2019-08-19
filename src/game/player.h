#pragma once

#include "base_entity.h"
#include "components/movement.h"
#include "components/stateful.h"
#include "components/animation.h"

class Player : public BaseEntity {
  public:
    void loadFromJson(nlohmann::json j) override;
};

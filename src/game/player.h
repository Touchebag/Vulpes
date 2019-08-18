#pragma once

#include "base_entity.h"
#include "attributes/movable.h"
#include "attributes/stateful.h"
#include "attributes/animated.h"

class Player : public BaseEntity {
  public:
    void loadFromJson(nlohmann::json j) override;
};

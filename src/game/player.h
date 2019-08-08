#pragma once

#include "base_entity.h"
#include "attributes/movable.h"
#include "attributes/stateful.h"
#include "attributes/animated.h"

class Player : public BaseEntity,
               public StatefulEntity,
               public AnimatedEntity {
  public:
    void update() override;
    void loadFromJson(nlohmann::json j) override;
    std::optional<nlohmann::json> outputToJson() override;
};

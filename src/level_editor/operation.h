#pragma once

#include "base_entity.h"
#include "system/world.h"

class Operation {
  public:
    void undo();
    void redo();

    std::shared_ptr<BaseEntity> entity_;

    std::optional<nlohmann::json> before_ = std::nullopt;
    std::optional<nlohmann::json> after_ = std::nullopt;
};

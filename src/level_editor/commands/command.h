#pragma once

#include "base_entity.h"
#include "world.h"

namespace command {

class Command {
  public:
    virtual void undo() {};
    virtual void redo() {};

    virtual ~Command() {};

    std::shared_ptr<BaseEntity> entity_;

    World::Layer layer_;

    std::pair<int, int> before_ = {0, 0};
    std::pair<int, int> after_ = {0, 0};
};

} // command

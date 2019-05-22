#pragma once

#include "base_entity.h"

namespace command {

class Command {
  public:
    virtual void undo() {};
    virtual void redo() {};

    virtual ~Command() {};

    std::shared_ptr<BaseEntity> entity_;

    std::pair<util::X, util::Y> before_ = {util::X(0), util::Y(0)};
    std::pair<util::X, util::Y> after_ = {util::X(0), util::Y(0)};
};

} // command

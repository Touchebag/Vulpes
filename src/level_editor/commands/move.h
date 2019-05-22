#pragma once

#include "command.h"

namespace command {

class Move : public Command {
  public:
    void undo() override;
    void redo() override;

    std::pair<util::X, util::Y> from_ = {util::X(0), util::Y(0)};
    std::pair<util::X, util::Y> to_ = {util::X(0), util::Y(0)};
};

} // command

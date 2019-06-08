#pragma once

#include "command.h"

namespace command {

class Move : public Command {
  public:
    void undo() override;
    void redo() override;
};

} // command
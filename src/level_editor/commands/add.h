#pragma once

#include "command.h"

namespace command {

class Add : public Command {
  public:
    void undo() override;
    void redo() override;
};

} // command

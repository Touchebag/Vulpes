#pragma once

#include "command.h"

namespace command {

class Resize : public Command {
  public:
    void undo() override;
    void redo() override;
};

} // command

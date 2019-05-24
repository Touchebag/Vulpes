#pragma once

#include "command.h"

namespace command {

class Delete : public Command {
  public:
    void undo() override;
    void redo() override;
};

} // command

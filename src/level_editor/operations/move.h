#pragma once

#include "operation.h"

namespace operation {

class Move : public Operation {
  public:
    void undo() override;
    void redo() override;
};

} // operation

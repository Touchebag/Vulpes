#pragma once

#include "operation.h"

namespace operation {

class Resize : public Operation {
  public:
    void undo() override;
    void redo() override;
};

} // operation

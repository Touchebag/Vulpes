#pragma once

#include "operation.h"

namespace operation {

class Add : public Operation {
  public:
    void undo() override;
    void redo() override;
};

} // operation

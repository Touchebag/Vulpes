#pragma once

#include "operation.h"

namespace operation {

class Remove : public Operation {
  public:
    void undo() override;
    void redo() override;
};

} // operation

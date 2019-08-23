#pragma once

#include "operation.h"

namespace operation {

class Delete : public Operation {
  public:
    void undo() override;
    void redo() override;
};

} // operation

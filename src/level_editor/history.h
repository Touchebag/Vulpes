#pragma once

#include <stack>

#include "operation.h"

class History {
  public:
    void undo();
    void redo();

    void addOperation(std::shared_ptr<Operation> command);

  private:
    std::stack<std::shared_ptr<Operation>> undo_history_;
    std::stack<std::shared_ptr<Operation>> redo_history_;
};

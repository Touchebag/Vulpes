#pragma once

#include <stack>

#include "commands/command.h"

class History {
  public:
    void undo();
    void redo();

    void addCommand(std::shared_ptr<command::Command> command);

  private:
    std::stack<std::shared_ptr<command::Command>> undo_history_;
    std::stack<std::shared_ptr<command::Command>> redo_history_;
};

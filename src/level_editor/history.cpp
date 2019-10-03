#include "history.h"

void History::undo() {
    if (!undo_history_.empty()) {
        auto cmd = undo_history_.top();

        cmd->undo();

        redo_history_.push(cmd);
        undo_history_.pop();
    }
}

void History::redo() {
    if (!redo_history_.empty()) {
        auto cmd = redo_history_.top();

        cmd->redo();

        undo_history_.push(cmd);
        redo_history_.pop();
    }
}

void History::addOperation(std::shared_ptr<Operation> command) {
    undo_history_.push(command);

    // Clear redo history if new command is added
    while (!redo_history_.empty()) {
        redo_history_.pop();
    }
}

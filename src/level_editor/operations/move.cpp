#include "move.h"

namespace operation {

void Move::undo() {
    entity_->setPosition(before_.first, before_.second);
}

void Move::redo() {
    entity_->setPosition(after_.first, after_.second);
}

} //operation

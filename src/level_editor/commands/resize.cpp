#include "resize.h"

namespace command {

void Resize::undo() {
    entity_->setHitbox(before_.first, before_.second);
}

void Resize::redo() {
    entity_->setHitbox(after_.first, after_.second);
}

} //command

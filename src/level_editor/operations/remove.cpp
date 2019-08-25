#include "remove.h"

namespace operation {

void Remove::undo() {
    World::getInstance().addEntity(entity_, layer_);
}

void Remove::redo() {
    World::getInstance().removeEntity(entity_, layer_);
}

} //operation

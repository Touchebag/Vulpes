#include "add.h"

namespace operation {

void Add::undo() {
    World::getInstance().removeEntity(entity_, layer_);
}

void Add::redo() {
    World::getInstance().addEntity(entity_, layer_);
}

} //operation

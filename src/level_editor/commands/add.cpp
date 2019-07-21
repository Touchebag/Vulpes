#include "add.h"

namespace command {

void Add::undo() {
    World::getInstance().removeEntity(entity_, layer_);
}

void Add::redo() {
    World::getInstance().addEntity(entity_, layer_);
}

} //command

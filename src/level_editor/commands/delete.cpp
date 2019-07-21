#include "delete.h"

namespace command {

void Delete::undo() {
    World::getInstance().addEntity(entity_, layer_);
}

void Delete::redo() {
    World::getInstance().removeEntity(entity_, layer_);
}

} //command

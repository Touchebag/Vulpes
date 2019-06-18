#include "delete.h"
#include "world.h"

namespace command {

void Delete::undo() {
    Render::getInstance().addEntity(entity_, layer_);
}

void Delete::redo() {
    Render::getInstance().removeEntity(entity_, layer_);
}

} //command

#include "add.h"
#include "world.h"

namespace command {

void Add::undo() {
    Render::getInstance().removeEntity(entity_, layer_);
}

void Add::redo() {
    Render::getInstance().addEntity(entity_, layer_);
}

} //command

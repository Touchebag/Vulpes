#include "add.h"
#include "world.h"

namespace command {

void Add::undo() {
    auto& world_objects = World::getInstance().getWorldObjects();

    world_objects.erase(std::remove(world_objects.begin(), world_objects.end(), entity_), world_objects.end());
}

void Add::redo() {
    World::getInstance().getWorldObjects().push_back(entity_);
}

} //command

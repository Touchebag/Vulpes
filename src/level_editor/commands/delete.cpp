#include "delete.h"
#include "world.h"

#include "log.h"

namespace command {

void Delete::undo() {
    World::getInstance().getWorldObjects().push_back(entity_);
}

void Delete::redo() {
    auto world_objects = World::getInstance().getWorldObjects();
    world_objects.erase(std::remove(world_objects.begin(), world_objects.end(), entity_), world_objects.end());
}

} //command

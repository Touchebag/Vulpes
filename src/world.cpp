#include "world.h"

World& World::getInstance() {
    static World world;

    return world;
}

std::vector<std::shared_ptr<BaseEntity>>& World::getWorldObjects() {
    return worldObjects;
}

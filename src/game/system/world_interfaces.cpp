#include "world.h"

const std::vector<std::weak_ptr<const Collision>>& World::IWorldRead::getCollisions(Collision::CollisionType coll_type) {
    return World::getWorldInstance().collisions_[static_cast<int>(coll_type)];
}

util::Point World::IWorldRead::getPlayerPosition() {
    return World::getWorldInstance().getPlayerPosition();
}

std::weak_ptr<Player> World::IWorldRead::getPlayer() {
    return World::getWorldInstance().getPlayer();
}


void World::IWorldModify::addEntity(std::shared_ptr<BaseEntity> entity) {
    World::getWorldInstance().addEntity(entity);
}

void World::IWorldModify::removeEntity(std::shared_ptr<BaseEntity> entity) {
    World::getWorldInstance().removeEntity(entity);
}

void World::IWorldModify::loadWorldFromJson(nlohmann::json j) {
    World::getWorldInstance().loadWorldFromJson(j);
}

void World::IWorldModify::loadWorldFromFile(std::string file) {
    World::getWorldInstance().loadWorldFromFile(file);
}

nlohmann::json World::IWorldModify::saveWorldToJson() {
    return World::getWorldInstance().saveWorldToJson();
}

void World::IWorldModify::saveWorldToFile(std::string file) {
    World::getWorldInstance().saveWorldToFile(file);
}

void World::IWorldModify::update() {
    World::getWorldInstance().update();
}

void World::IWorldModify::clearWorld() {
    World::getWorldInstance().clearWorld();
}

std::vector<std::shared_ptr<BaseEntity>>& World::IWorldModify::getWorldObjects() {
    return World::getWorldInstance().getWorldObjects();
}


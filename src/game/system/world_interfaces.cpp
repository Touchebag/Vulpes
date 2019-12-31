#include "world.h"

std::vector<std::shared_ptr<BaseEntity>>& World::IWorldRead::getWorldObjects(World::Layer layer) {
    return World::getWorldInstance().getWorldObjects(layer);
}

util::Point World::IWorldRead::getPlayerPosition() {
    return World::getWorldInstance().getPlayerPosition();
}

std::weak_ptr<Player> World::IWorldRead::getPlayer() {
    return World::getWorldInstance().getPlayer();
}


void World::IWorldModify::addEntity(std::shared_ptr<BaseEntity> entity, World::Layer layer) {
    World::getWorldInstance().addEntity(entity, layer);
}

void World::IWorldModify::removeEntity(std::shared_ptr<BaseEntity> entity, World::Layer layer) {
    World::getWorldInstance().removeEntity(entity, layer);
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

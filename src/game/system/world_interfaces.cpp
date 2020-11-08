#include "world.h"

#include "utils/log.h"

/*--- Read ---*/

const std::vector<std::weak_ptr<const Collision>>& World::IWorldRead::getCollisions(Collision::CollisionType coll_type) {
    return World::getWorldInstance().collisions_[static_cast<int>(coll_type)];
}

util::Point World::IWorldRead::getPlayerPosition() {
    return World::getWorldInstance().getPlayerPosition();
}

std::weak_ptr<const Player> World::IWorldRead::getPlayer() {
    return World::getWorldInstance().getPlayer();
}

bool World::IWorldRead::hasInteractTriggered() {
    return World::getWorldInstance().interact_triggered_;
}

std::string World::IWorldRead::getCurrentRoomName() {
    return World::getWorldInstance().current_room_name_;
}

/*--- Modify ---*/

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

std::weak_ptr<Player> World::IWorldModify::getPlayer() {
    return World::getWorldInstance().getPlayer();
}

void World::IWorldModify::triggerInterract() {
    World::getWorldInstance().interact_triggered_ = true;
}

void World::IWorldModify::loadRoom(std::string room_name, int entrance_id) {
    // Enqueue new room load at frame end
    World::getWorldInstance().new_room_ = {room_name, entrance_id};
}

void World::IWorldModify::setEntrance(int entrance_id) {
    World::getWorldInstance().setEntrance(entrance_id);
}

void World::IWorldModify::loadCameraData(IRender::CameraBox camera_box) {
    World::getWorldInstance().camera_box_ = camera_box;
}

#include "system.h"

void System::IWorldModify::addEntity(std::shared_ptr<BaseEntity> entity, std::optional<std::string> condition) {
    // TODO Iterator invalidation
    System::getInstance().world_->addEntity(entity, condition);
}

void System::IWorldModify::removeEntity(std::shared_ptr<BaseEntity> entity) {
    System::getInstance().world_->removeEntity(entity);
}

void System::IWorldModify::addCollideable(std::shared_ptr<Collideable> collideable) {
    System::getInstance().world_->addCollideable(collideable);
}

void System::IWorldModify::loadWorldFromJson(nlohmann::json j) {
    System::getInstance().world_->loadWorldFromJson(j);
}

void System::IWorldModify::loadWorldFromFile(const std::string& file) {
    System::getInstance().world_->loadWorldFromFile(file);
}

nlohmann::json System::IWorldModify::saveWorldToJson() {
    return System::getInstance().world_->saveWorldToJson();
}

void System::IWorldModify::saveWorldToFile(const std::string& file) {
    System::getInstance().world_->saveWorldToFile(file);
}

void System::IWorldModify::update() {
    System::getInstance().world_->update();
}

void System::IWorldModify::clearWorld() {
    System::getInstance().world_->clearWorld();
}

std::vector<std::shared_ptr<BaseEntity>>& System::IWorldModify::getWorldObjects() {
    return System::getInstance().world_->getWorldObjects();
}

std::weak_ptr<Player> System::IWorldModify::getPlayer() {
    return System::getInstance().world_->getPlayer();
}

std::map<std::string, std::shared_ptr<BaseEntity>> System::IWorldModify::getEntitesByTags(const std::set<std::string>& tags) {
    return System::getInstance().world_->getEntitesByTags(tags);
}

void System::IWorldModify::loadRoom(std::string room_name, unsigned int entrance_id) {
    // Enqueue new room load at frame end
    System::getInstance().world_->new_room_ = {room_name, entrance_id};
}

void System::IWorldModify::setEntrance(unsigned int entrance_id) {
    System::getInstance().world_->setEntrance(entrance_id);
}

void System::IWorldModify::loadCameraData(const Camera::CameraBoundingBox& camera_box) {
    System::getInstance().world_->camera_box_ = camera_box;
}

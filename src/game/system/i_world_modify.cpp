#include "system.h"

void System::IWorldModify::addEntity(std::shared_ptr<BaseEntity> entity, std::optional<std::string> condition) {
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

void System::IWorldModify::loadWorldFromFile(std::string file) {
    System::getInstance().world_->loadWorldFromFile(file);
}

nlohmann::json System::IWorldModify::saveWorldToJson() {
    return System::getInstance().world_->saveWorldToJson();
}

void System::IWorldModify::saveWorldToFile(std::string file) {
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

void System::IWorldModify::triggerInterract() {
    System::getInstance().world_->interact_triggered_ = true;
}

void System::IWorldModify::loadRoom(std::string room_name, int entrance_id) {
    // Enqueue new room load at frame end
    System::getInstance().world_->new_room_ = {room_name, entrance_id};
}

void System::IWorldModify::setEntrance(int entrance_id) {
    System::getInstance().world_->setEntrance(entrance_id);
}

void System::IWorldModify::loadCameraData(Camera::CameraBoundingBox camera_box) {
    System::getInstance().world_->camera_box_ = camera_box;
}

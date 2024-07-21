#include "system.h"

const std::vector<std::weak_ptr<const Collideable>>& System::IWorldRead::getCollideables(Collideable::CollisionType coll_type) {
    return System::getInstance().world_->collideables_.at(coll_type);
}

util::Vec2i System::IWorldRead::getPlayerPosition() {
    return System::getInstance().world_->getPlayerPosition();
}

std::weak_ptr<const Player> System::IWorldRead::getPlayer() {
    return System::getInstance().world_->getPlayer();
}

std::string System::IWorldRead::getCurrentRoomName() {
    return System::getInstance().world_->current_room_name_;
}

std::vector<util::Vec2i> System::IWorldRead::getEntrances() {
    return System::getInstance().world_->entrances_;
}

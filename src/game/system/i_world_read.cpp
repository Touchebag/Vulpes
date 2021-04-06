#include "system.h"

const std::vector<std::weak_ptr<const Collideable>>& System::IWorldRead::getCollideables(Collideable::CollisionType coll_type) {
    return System::getInstance().world_->collideables_[static_cast<int>(coll_type)];
}

util::Point System::IWorldRead::getPlayerPosition() {
    return System::getInstance().world_->getPlayerPosition();
}

std::weak_ptr<const Player> System::IWorldRead::getPlayer() {
    return System::getInstance().world_->getPlayer();
}

bool System::IWorldRead::hasInteractTriggered() {
    return System::getInstance().world_->interact_triggered_;
}

std::string System::IWorldRead::getCurrentRoomName() {
    return System::getInstance().world_->current_room_name_;
}

std::vector<util::Point> System::IWorldRead::getEntrances() {
    return System::getInstance().world_->entrances_;
}

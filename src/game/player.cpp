#include "player.h"
#include "system/input_event.h"
#include "utils/log.h"

#include <tuple>
#include <algorithm>

void Player::loadFromJson(nlohmann::json j) {
    BaseEntity::loadFromJson(j);

    if (actions_) {
        Input::getInstance().setActionsInstance(actions_);
    }
}

void Player::update() {
    if (actions_) {
        actions_->updateActions();
    }

    BaseEntity::update();
}

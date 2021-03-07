#include "player.h"
#include "system/input_event.h"
#include "utils/log.h"

#include <tuple>
#include <algorithm>

std::shared_ptr<Player> Player::createFromJson(const nlohmann::json& j) {
    auto ret_ptr = std::make_shared<Player>();

    ret_ptr->reloadFromJson(j);

    return ret_ptr;
}

void Player::reloadFromJson(const nlohmann::json& j) {
    BaseEntity::reloadFromJson(j);

    if (actions_) {
        Input::getInstance().setActionsInstance(actions_);
    }
}

void Player::update() {
    BaseEntity::update();
}

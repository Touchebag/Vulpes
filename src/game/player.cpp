#include "player.h"
#include "system/system.h"
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

    if (getComponent<Actions>()) {
        System::getInput()->setActionsInstance(getComponent<Actions>());
    }
}

void Player::update() {
    BaseEntity::update();
}

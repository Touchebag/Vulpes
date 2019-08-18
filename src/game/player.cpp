#include "player.h"
#include "input_event.h"
#include "log.h"

#include <tuple>
#include <algorithm>

void Player::loadFromJson(nlohmann::json j) {
    BaseEntity::loadFromJson(j);

    if (actions_) {
        Input::getInstance().setActionsInstance(actions_);
    }

    // TODO Move to somehwere relevant
    if (auto stateEnt = statefulEntity_) {
        stateEnt->incomingEvent(state::Event::START);
    }
}

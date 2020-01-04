#include "ai.h"

#include "utils/log.h"

AI::AI(std::weak_ptr<Actions> actions) :
    actions_(actions) {
}

void AI::update() {
    if (auto act = actions_.lock()) {
        act->addAction(Actions::Action::MOVE_LEFT);
    }
}

void AI::loadFromJson(nlohmann::json j) {
}

std::optional<nlohmann::json> AI::outputToJson() {
    return {};
}


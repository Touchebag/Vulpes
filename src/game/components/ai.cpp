#include "ai.h"

#include "utils/log.h"
#include "system/world.h"

AI::AI(std::weak_ptr<Actions> actions, std::weak_ptr<Transform> transform) :
    actions_(actions),
    transform_(transform) {
}

void AI::update() {
    auto act = actions_.lock();
    auto trans = transform_.lock();

    if (act && trans) {
        if (World::IWorldRead::getPlayerPosition().x > trans->getX()) {
            act->addAction(Actions::Action::MOVE_RIGHT);

            // TODO Figure out how to clear actions correctly
            act->removeAction(Actions::Action::MOVE_LEFT);
        } else {
            act->addAction(Actions::Action::MOVE_LEFT);

            // TODO Figure out how to clear actions correctly
            act->removeAction(Actions::Action::MOVE_RIGHT);
        }
    }
}

void AI::loadFromJson(nlohmann::json j) {
}

std::optional<nlohmann::json> AI::outputToJson() {
    return {};
}


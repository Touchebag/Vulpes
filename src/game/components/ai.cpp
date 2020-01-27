#include "ai.h"

#include "utils/log.h"
#include "system/world.h"

#include "ai/logic_operators/greater.h"
#include "ai/logic_operators/less.h"
#include "ai/values/constant.h"
#include "ai/values/dynamic.h"

AI::AI(std::weak_ptr<Actions> actions, std::weak_ptr<Transform> transform) :
    actions_(actions),
    transform_(transform) {
}

void AI::update() {
    auto act = actions_.lock();
    auto trans = transform_.lock();

    auto v1 = ai::condition::Dynamic({});
    auto v2 = ai::condition::Constant(500);

    auto grt = ai::condition::Greater(v1, v2);
    auto lss = ai::condition::Less(v1, v2);

    if (act && trans) {
        if (grt) {
            act->addAction(Actions::Action::MOVE_RIGHT);

            // TODO Figure out how to clear actions correctly
            act->removeAction(Actions::Action::MOVE_LEFT);
        } else if (lss) {
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


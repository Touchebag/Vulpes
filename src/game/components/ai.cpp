#include "ai.h"

#include "utils/log.h"
#include "system/world.h"

#include "ai/logic_operators/greater.h"
#include "ai/logic_operators/less.h"
#include "ai/values/constant.h"
#include "ai/values/dynamic.h"
#include "ai/values/this.h"

AI::AI(std::weak_ptr<Actions> actions, std::weak_ptr<Transform> transform) :
    actions_(actions),
    transform_(transform) {
}

void AI::update() {
    auto act = actions_.lock();
    auto trans = transform_.lock();

    nlohmann::json j1 = nlohmann::json::parse( R"--(
{
    "type": "grt",
    "lhs": {
        "type": "dynamic",
        "value": "player.position.x"
    },
    "rhs": {
        "type": "this",
        "value": "position.x"
    }
}
)--");

    nlohmann::json j2 = nlohmann::json::parse( R"--(
{
    "type": "lss",
    "lhs": {
        "type": "dynamic",
        "value": "player.position.x"
    },
    "rhs": {
        "type": "this",
        "value": "position.x"
    }
}
)--");

    auto grt = ai::condition::LogicalOperator::loadFromJson(j1, this->transform_);
    auto lss = ai::condition::LogicalOperator::loadFromJson(j2, this->transform_);

    if (act && trans) {
        if (grt->getValue()) {
            act->addAction(Actions::Action::MOVE_RIGHT);
        } else if (lss->getValue()) {
            act->addAction(Actions::Action::MOVE_LEFT);
        }
    }
}

void AI::loadFromJson(nlohmann::json j) {
}

std::optional<nlohmann::json> AI::outputToJson() {
    return {};
}


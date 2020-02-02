#include "ai.h"

#include "utils/log.h"
#include "utils/file.h"
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

    if (act && trans) {
        for (auto& it : states_.getStateData()) {
            if (it.first->getValue(trans)) {
                act->addAction(it.second);
            }
        }
    }
}

void AI::loadFromJson(nlohmann::json j) {
    if (auto ai_json = File::loadAiBehavior("basic_enemy.json")) {
        states_ = StateHandler<std::vector<std::pair<std::shared_ptr<const ai::condition::LogicalOperator>, Actions::Action>>>();

        auto ai_behavior = ai_json.value();
        states_.loadFromJson(ai_behavior);
    }
}

std::optional<nlohmann::json> AI::outputToJson() {
    return {};
}


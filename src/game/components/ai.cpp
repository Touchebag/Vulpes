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

    frame_timer_++;

    if (act && trans) {
        for (auto& it : states_.getStateData()) {
            if (it.first->getValue(trans, frame_timer_)) {
                auto action = it.second;
                switch (action) {
                    // AI_EVENTs should be treated differentely from input actions
                    case Actions::Action::AI_EVENT_1:
                        states_.incomingEvent(state_utils::Event::AI_EVENT_1);
                        break;
                    case Actions::Action::AI_EVENT_2:
                        states_.incomingEvent(state_utils::Event::AI_EVENT_2);
                        break;
                    default:
                        act->addAction(it.second);
                        break;
                }
            }
        }
    }
}

void AI::reloadFromJson(nlohmann::json j) {
    if (j.contains("file")) {
        if (auto ai_json = File::loadAiBehavior(j["file"])) {
            states_ = StateHandler<std::vector<std::pair<std::shared_ptr<const ai::condition::LogicalOperator>, Actions::Action>>>();

            auto ai_behavior = ai_json.value();
            states_.reloadFromJson(ai_behavior);
        }
    } else {
        throw std::invalid_argument("Cannot find AI file");
    }
}

std::optional<nlohmann::json> AI::outputToJson() {
    return {};
}


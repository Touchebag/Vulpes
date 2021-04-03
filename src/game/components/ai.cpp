#include "ai.h"

#include "utils/log.h"
#include "utils/file.h"
#include "utils/common.h"

#include "system/world.h"

#include "ai/logic_operators/greater.h"
#include "ai/logic_operators/less.h"
#include "ai/values/constant.h"
#include "ai/values/player_value.h"
#include "ai/values/this.h"

AI::AI(std::weak_ptr<ComponentStore> components) :
    Component(components) {
}

void AI::update() {
    auto act = component_store_.lock()->actions;

    frame_timer_++;

    if (act) {
        ai::condition::LogicalOperator::aiValues values {
            component_store_.lock()->transform,
            component_store_.lock()->collision,
            component_store_.lock()->animatedEntity,
            frame_timer_
        };

        for (auto& it : states_.getStateData()) {
            if (it.first->getValue(values)) {
                auto action = it.second;
                switch (action) {
                    // AI_EVENTs should be treated differently from input actions
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
    } else {
        LOGW("AI: Missing actions");
    }
}

std::shared_ptr<AI> AI::createFromJson(nlohmann::json j, std::weak_ptr<ComponentStore> components, File file_instance) {
    auto ret_ptr = std::make_shared<AI>(components);

    ret_ptr->reloadFromJson(j, file_instance);

    return ret_ptr;
}

void AI::reloadFromJson(nlohmann::json /* j */, File file) {
    if (auto ai_json = file.loadAiBehavior()) {
        states_ = StateHandler<std::vector<std::pair<std::shared_ptr<const ai::condition::LogicalOperator>, Actions::Action>>>();

        auto ai_behavior = ai_json.value();
        states_.reloadFromJson(ai_behavior);
    }
}

std::optional<nlohmann::json> AI::outputToJson() {
    nlohmann::json j;

    return j;
}


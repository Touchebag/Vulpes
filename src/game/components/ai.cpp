#include "ai.h"

#include "utils/log.h"
#include "utils/bimap.h"
#include "utils/file.h"
#include "utils/common.h"

#include "system/system.h"

namespace {

const Bimap<Actions::Action, state_utils::Event> action_event_map = {
    #define GENERATE_ENUM(action, name) {Actions::Action::action, state_utils::Event::ACTION_##action},
    #include "components/actions/actions_enum.h"
    #undef GENERATE_ENUM
};

} // namespace

AI::AI(std::weak_ptr<ComponentStore> components) :
    Component(components) {
}

bool AI::evaluateCondition(std::vector<int> condition) {
    return true;
}

void AI::update() {
    auto act = getComponent<Actions>();

    frame_timer_++;

    if (act) {
        for (auto& it : states_.getStateData()) {
            if (evaluateCondition(it.first)) {
                // Add resulting action
                auto action = it.second;
                act->addAction(action);

                if (action != Actions::Action::DIE) {
                    // Update any effects of said action
                    // (e.g. AI actions)
                    auto corresponding_event = action_event_map.at(action);
                    states_.incomingEvent(corresponding_event);
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
        states_ = StateHandler<std::vector<std::pair<std::vector<int>, Actions::Action>>>();

        auto ai_behavior = ai_json.value();
        states_.reloadFromJson(ai_behavior);
    }
}

std::optional<nlohmann::json> AI::outputToJson() {
    nlohmann::json j;

    return j;
}


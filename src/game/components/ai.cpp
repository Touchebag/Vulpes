#include "ai.h"

#include "utils/log.h"
#include "utils/bimap.h"
#include "utils/file.h"
#include "utils/common.h"

#include "ai/interpreter.h"
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

void AI::update() {
    auto act = getComponent<Actions>();

    frame_timer_++;

    if (act) {
        Interpreter::ExtraInputData extra_data;
        extra_data.frame_timer = frame_timer_;
        extra_data.this_components = component_store_.lock();

        for (auto& it : states_.getStateData()) {
            if (Interpreter::executeProgram(it.first, extra_data) == ai::Bool::TRUE) {
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
        states_ = StateHandler<std::vector<STATE_AI_CONDITION_TYPE>>();

        auto ai_behavior = ai_json.value();
        states_.reloadFromJson(ai_behavior);
    }
}

std::optional<nlohmann::json> AI::outputToJson() {
    nlohmann::json j;

    return j;
}


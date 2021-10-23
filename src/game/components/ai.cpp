#include "ai.h"

#include "utils/log.h"
#include "utils/bimap.h"
#include "utils/file.h"
#include "utils/common.h"

#include "ai/interpreter.h"
#include "system/system.h"

AI::AI(std::weak_ptr<ComponentStore> components) :
    Component(components) {
}

void AI::update() {
    frame_timer_++;

    Interpreter::ExtraInputData extra_data;
    extra_data.frame_timer = frame_timer_;
    extra_data.this_components = component_store_.lock();

    for (auto& it : states_.getStateData()) {
        Interpreter::executeProgram(it, extra_data);
    }
}

std::shared_ptr<AI> AI::createFromJson(nlohmann::json j, std::weak_ptr<ComponentStore> components, File file_instance) {
    auto ret_ptr = std::make_shared<AI>(components);

    ret_ptr->reloadFromJson(j, file_instance);

    return ret_ptr;
}

void AI::reloadFromJson(nlohmann::json /* j */, File file) {
    if (auto ai_json = file.loadAiBehavior()) {
        states_ = StateHandler<STATE_AI_CONDITION_TYPE>();

        auto ai_behavior = ai_json.value();
        states_.reloadFromJson(ai_behavior);
    }
}

std::optional<nlohmann::json> AI::outputToJson() {
    nlohmann::json j;

    return j;
}


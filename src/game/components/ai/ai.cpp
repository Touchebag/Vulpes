#include "ai.h"

#include "utils/log.h"
#include "utils/bimap.h"
#include "utils/file.h"
#include "utils/common.h"

#include "interpreter.h"
#include "system/system.h"

AI::AI(std::weak_ptr<ComponentStore> components) :
    Component(components) {
}

void AI::update() {
    if (auto state = getComponent<Stateful>()) {
        frame_timer_++;

        Interpreter::ExtraInputData extra_data;
        extra_data.frame_timer = frame_timer_;
        extra_data.this_components = component_store_.lock();

        for (auto& it : state->getAI()) {
            Interpreter::executeProgram(it, extra_data);
        }
    }
}

void AI::executeProgram(const Program& program) {
    Interpreter::ExtraInputData extra_data;
    extra_data.frame_timer = frame_timer_;
    extra_data.this_components = component_store_.lock();

    Interpreter::executeProgram(program, extra_data);
}

std::shared_ptr<AI> AI::createFromJson(nlohmann::json j, std::weak_ptr<ComponentStore> components, File file_instance) {
    auto ret_ptr = std::make_shared<AI>(components);

    ret_ptr->reloadFromJson(j, file_instance);

    return ret_ptr;
}

void AI::reloadFromJson(nlohmann::json /* j */, File /* file */) {
}

std::optional<nlohmann::json> AI::outputToJson() {
    nlohmann::json j;

    return j;
}


#include "script.h"

#include "utils/log.h"
#include "utils/bimap.h"
#include "utils/file.h"
#include "utils/common.h"

#include "system/system.h"

Scripting::Scripting(std::weak_ptr<ComponentStore> components) :
    Component(components) {
}

void Scripting::update() {
    if (auto state = getComponent<Stateful>()) {
        frame_timer_++;

        Program::ExtraInputData extra_data;
        extra_data.frame_timer = frame_timer_;
        extra_data.this_components = component_store_.lock();

        for (auto& it : state->getScript()) {
            it.run(extra_data);
        }
    }
}

void Scripting::executeProgram(Program& program) {
    Program::ExtraInputData extra_data;
    extra_data.frame_timer = frame_timer_;
    extra_data.this_components = component_store_.lock();
    extra_data.variables = variables_;

    program.run(extra_data);
}

std::shared_ptr<Scripting> Scripting::createFromJson(nlohmann::json j, std::weak_ptr<ComponentStore> components, File file_instance) {
    auto ret_ptr = std::make_shared<Scripting>(components);

    ret_ptr->reloadFromJson(j, file_instance);

    return ret_ptr;
}

void Scripting::reloadFromJson(nlohmann::json /* j */, File /* file */) {
}

std::optional<nlohmann::json> Scripting::outputToJson() {
    nlohmann::json j;

    return j;
}


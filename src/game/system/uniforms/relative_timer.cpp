#include "relative_timer.h"

#include "system/system.h"

RelativeTimer::RelativeTimer(std::string uniform_name) :
    IShaderUniform(uniform_name) {
}

void RelativeTimer::applyUniform(std::shared_ptr<sf::Shader> shader) {
    shader->setUniform(uniform_name_, frame_timer_++);
}

std::shared_ptr<RelativeTimer> RelativeTimer::createFromJson(nlohmann::json j) {
    auto uniform = std::make_shared<RelativeTimer>(j["name"]);

    return uniform;
}

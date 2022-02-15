#include "relative_timer.h"

#include "system/system.h"

RelativeTimer::RelativeTimer(const std::string& uniform_name) :
    IShaderUniform(uniform_name) {
    start_time_.restart();
}

void RelativeTimer::applyUniform(std::shared_ptr<sf::Shader> shader, std::shared_ptr<ComponentStore>) {
    auto time_diff = start_time_.getElapsedTime();
    shader->setUniform(uniform_name_, time_diff.asSeconds());
}

std::shared_ptr<RelativeTimer> RelativeTimer::createFromJson(nlohmann::json j) {
    auto uniform = std::make_shared<RelativeTimer>(j["name"]);

    return uniform;
}

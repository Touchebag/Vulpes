#include "relative_position.h"

#include "system/system.h"

#include "utils/log.h"

RelativePosition::RelativePosition(std::string uniform_name) :
    IShaderUniform(uniform_name) {
}

void RelativePosition::applyUniform(std::shared_ptr<sf::Shader> shader, std::shared_ptr<ComponentStore> components) {
    if (components) {
        if (auto trans = components->getComponent<Transform>()) {
            auto pos = trans->getPosition();

            auto rel_pos = System::getCamera()->getRelativePosition(pos.x, pos.y);

            position_ = {static_cast<float>(rel_pos.first), static_cast<float>(rel_pos.second)};
        }
    }

    shader->setUniform(uniform_name_, position_);
}

std::shared_ptr<RelativePosition> RelativePosition::createFromJson(nlohmann::json j) {
    auto uniform = std::make_shared<RelativePosition>(j["name"]);

    return uniform;
}

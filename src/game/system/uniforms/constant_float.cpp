#include "constant_float.h"

ConstantFloat::ConstantFloat(std::string uniform_name) :
    IShaderUniform(uniform_name) {
}

void ConstantFloat::applyUniform(std::shared_ptr<sf::Shader> shader, std::shared_ptr<ComponentStore>) {
    shader->setUniform(uniform_name_, value_);
}

std::shared_ptr<ConstantFloat> ConstantFloat::createFromJson(nlohmann::json j) {
    if (!j.contains("value")) {
        return nullptr;
    }

    auto uniform = std::make_shared<ConstantFloat>(j["name"]);

    uniform->value_ = j["value"];

    return uniform;
}

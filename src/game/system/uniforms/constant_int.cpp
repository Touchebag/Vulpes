#include "constant_int.h"

#include "utils/log.h"

ConstantInt::ConstantInt(std::string uniform_name) :
    IShaderUniform(uniform_name) {
}

void ConstantInt::applyUniform(std::shared_ptr<sf::Shader> shader) {
    shader->setUniform(uniform_name_, value_);
}

std::shared_ptr<ConstantInt> ConstantInt::createFromJson(nlohmann::json j) {
    if (!j.contains("value")) {
        return nullptr;
    }

    auto uniform = std::make_shared<ConstantInt>(j["name"]);

    uniform->value_ = j["value"].get<int>();

    return uniform;
}

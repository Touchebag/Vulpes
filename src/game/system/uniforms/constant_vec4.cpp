#include "constant_vec4.h"

ConstantVec4::ConstantVec4(std::string uniform_name) :
    IShaderUniform(uniform_name) {
}

void ConstantVec4::applyUniform(std::shared_ptr<sf::Shader> shader) {
    shader->setUniform(uniform_name_, value_);
}

std::shared_ptr<ConstantVec4> ConstantVec4::createFromJson(nlohmann::json j) {
    if(!(j.contains("a") && j.contains("b") && j.contains("c") && j.contains("d"))) {
        return nullptr;
    }

    auto uniform = std::make_shared<ConstantVec4>(j["name"]);

    uniform->value_ = {j["a"], j["b"], j["c"], j["d"]};

    return uniform;
}

#include "constant_vec2.h"

ConstantVec2::ConstantVec2(std::string uniform_name) :
    IShaderUniform(uniform_name) {
}

void ConstantVec2::apply_uniform(std::shared_ptr<sf::Shader> shader) {
    shader->setUniform(uniform_name_, value_);
}

std::shared_ptr<ConstantVec2> ConstantVec2::createFromJson(nlohmann::json j) {
    if(!(j.contains("a") && j.contains("b"))) {
        return nullptr;
    }

    auto uniform = std::make_shared<ConstantVec2>(j["name"]);

    uniform->value_ = {j["a"], j["b"]};

    return uniform;
}

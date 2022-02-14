#pragma once

#include "nlohmann/json.hpp"

#include "i_shader_uniform.h"

class ConstantVec4 : public IShaderUniform {
  public:
    explicit ConstantVec4(std::string uniform_name);

    void applyUniform(std::shared_ptr<sf::Shader> shader, std::shared_ptr<ComponentStore> components) override;

    static std::shared_ptr<ConstantVec4> createFromJson(nlohmann::json j);

  private:
    sf::Glsl::Vec4 value_;
};

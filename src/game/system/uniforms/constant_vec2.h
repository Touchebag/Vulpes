#pragma once

#include "nlohmann/json.hpp"

#include "i_shader_uniform.h"

class ConstantVec2 : public IShaderUniform {
  public:
    explicit ConstantVec2(const std::string& uniform_name);

    void applyUniform(std::shared_ptr<sf::Shader> shader, std::shared_ptr<ComponentStore> components) override;

    static std::shared_ptr<ConstantVec2> createFromJson(nlohmann::json j);

  private:
    sf::Glsl::Vec2 value_;
};

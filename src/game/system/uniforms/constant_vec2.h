#pragma once

#include <json.hpp>

#include "i_shader_uniform.h"

class ConstantVec2 : public IShaderUniform {
  public:
    ConstantVec2(std::string uniform_name);

    void apply_uniform(std::shared_ptr<sf::Shader> shader) override;

    static std::shared_ptr<ConstantVec2> createFromJson(nlohmann::json j);

  private:
    sf::Glsl::Vec2 value_;
};

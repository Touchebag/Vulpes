#pragma once

#include <json.hpp>

#include "i_shader_uniform.h"

class ConstantVec4 : public IShaderUniform {
  public:
    ConstantVec4(std::string uniform_name);

    void apply_uniform(std::shared_ptr<sf::Shader> shader) override;

    static std::shared_ptr<ConstantVec4> createFromJson(nlohmann::json j);

  private:
    sf::Glsl::Vec4 value_;
};

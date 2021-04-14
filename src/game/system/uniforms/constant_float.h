#pragma once

#include "i_shader_uniform.h"

#include <json.hpp>

class ConstantFloat : public IShaderUniform {
  public:
    ConstantFloat(std::string uniform_name);

    void applyUniform(std::shared_ptr<sf::Shader> shader) override;

    static std::shared_ptr<ConstantFloat> createFromJson(nlohmann::json j);

  private:
    float value_;
};

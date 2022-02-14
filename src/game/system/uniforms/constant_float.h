#pragma once

#include "i_shader_uniform.h"

#include "nlohmann/json.hpp"

class ConstantFloat : public IShaderUniform {
  public:
    explicit ConstantFloat(std::string uniform_name);

    void applyUniform(std::shared_ptr<sf::Shader> shader, std::shared_ptr<ComponentStore> components) override;

    static std::shared_ptr<ConstantFloat> createFromJson(nlohmann::json j);

  private:
    float value_ = 0.0;
};

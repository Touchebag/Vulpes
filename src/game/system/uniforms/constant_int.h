#pragma once

#include "i_shader_uniform.h"

#include "nlohmann/json.hpp"

class ConstantInt : public IShaderUniform {
  public:
    ConstantInt(std::string uniform_name);

    void applyUniform(std::shared_ptr<sf::Shader> shader, std::shared_ptr<ComponentStore> components) override;

    static std::shared_ptr<ConstantInt> createFromJson(nlohmann::json j);

  private:
    int value_;
};

#pragma once

#include "i_shader_uniform.h"

#include "nlohmann/json.hpp"

class WindowSize : public IShaderUniform {
  public:
    explicit WindowSize(std::string uniform_name);

    void applyUniform(std::shared_ptr<sf::Shader> shader, std::shared_ptr<ComponentStore> components) override;

    static std::shared_ptr<WindowSize> createFromJson(nlohmann::json j);
};

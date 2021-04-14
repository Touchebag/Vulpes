#pragma once

#include "i_shader_uniform.h"

#include <json.hpp>

class WindowSize : public IShaderUniform {
  public:
    WindowSize(std::string uniform_name);

    void applyUniform(std::shared_ptr<sf::Shader> shader) override;

    static std::shared_ptr<WindowSize> createFromJson(nlohmann::json j);
};

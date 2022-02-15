#pragma once

#include "nlohmann/json.hpp"

#include "i_shader_uniform.h"

class RelativePosition : public IShaderUniform {
  public:
    explicit RelativePosition(const std::string& uniform_name);

    void applyUniform(std::shared_ptr<sf::Shader> shader, std::shared_ptr<ComponentStore> components) override;

    static std::shared_ptr<RelativePosition> createFromJson(nlohmann::json j);

  private:
    // Default to middle of screen
    sf::Glsl::Vec2 position_ = {0.5, 0.5};
};

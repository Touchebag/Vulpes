#pragma once

#include "i_shader_uniform.h"

#include "nlohmann/json.hpp"
#include <SFML/System.hpp>

class RelativeTimer : public IShaderUniform {
  public:
    explicit RelativeTimer(std::string uniform_name);

    void applyUniform(std::shared_ptr<sf::Shader> shader, std::shared_ptr<ComponentStore> components) override;

    static std::shared_ptr<RelativeTimer> createFromJson(nlohmann::json j);

  private:
    sf::Clock start_time_;
};

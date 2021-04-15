#pragma once

#include "i_shader_uniform.h"

#include <json.hpp>

class RelativeTimer : public IShaderUniform {
  public:
    RelativeTimer(std::string uniform_name);

    void applyUniform(std::shared_ptr<sf::Shader> shader) override;

    static std::shared_ptr<RelativeTimer> createFromJson(nlohmann::json j);

  private:
    int frame_timer_ = 0;
};

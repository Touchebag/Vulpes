#pragma once

#include <json.hpp>

#include "uniforms/i_shader_uniform.h"

class ShaderHandle {
  public:
    static ShaderHandle createFromJson(nlohmann::json j);

    ShaderHandle& update();
    sf::Shader* get();

  private:
    ShaderHandle(std::shared_ptr<sf::Shader> shader);

    std::shared_ptr<sf::Shader> shader_;

    std::vector<std::shared_ptr<IShaderUniform>> uniforms_;
};

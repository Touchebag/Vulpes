#pragma once

#include "nlohmann/json.hpp"

#include "uniforms/i_shader_uniform.h"

class ShaderHandle {
  public:
    static std::shared_ptr<ShaderHandle> createFromJson(nlohmann::json j);

    void update();
    sf::Shader* getShader();

  private:
    ShaderHandle(std::shared_ptr<sf::Shader> shader);

    std::shared_ptr<sf::Shader> shader_;

    std::vector<std::shared_ptr<IShaderUniform>> uniforms_;
};

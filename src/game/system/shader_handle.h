#pragma once

#include "nlohmann/json.hpp"

#include "uniforms/i_shader_uniform.h"

struct ComponentStore;

class ShaderHandle {
  public:
    static std::shared_ptr<ShaderHandle> createFromJson(nlohmann::json j, std::weak_ptr<ComponentStore> components);

    void update();
    sf::Shader* getShader();

  private:
    ShaderHandle(std::shared_ptr<sf::Shader> shader, std::weak_ptr<ComponentStore> components);

    std::shared_ptr<sf::Shader> shader_;

    std::vector<std::shared_ptr<IShaderUniform>> uniforms_;

    std::weak_ptr<ComponentStore> component_store_;
};

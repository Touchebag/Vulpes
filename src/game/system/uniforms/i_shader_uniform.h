#pragma once

#include <SFML/Graphics.hpp>

#include <memory>

class ComponentStore;

class IShaderUniform {
  public:
    inline IShaderUniform(const std::string& uniform_name) : uniform_name_(uniform_name) {};

    virtual ~IShaderUniform() = default;

    virtual void applyUniform(std::shared_ptr<sf::Shader> shader, std::shared_ptr<ComponentStore> components = nullptr) = 0;

  protected:
    std::string uniform_name_;
};

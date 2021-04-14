#pragma once

#include <SFML/Graphics.hpp>

#include <memory>

class IShaderUniform {
  public:
    inline IShaderUniform(std::string uniform_name) : uniform_name_(uniform_name) {};

    virtual ~IShaderUniform() = default;

    virtual void applyUniform(std::shared_ptr<sf::Shader> shader) = 0;

  protected:
    std::string uniform_name_;
};

#include "window_size.h"

#include "system/system.h"

WindowSize::WindowSize(const std::string& uniform_name) :
    IShaderUniform(uniform_name) {
}

void WindowSize::applyUniform(std::shared_ptr<sf::Shader> shader, std::shared_ptr<ComponentStore>) {
    auto window_size = System::getCamera()->getWindowSize();
    sf::Glsl::Vec2 size = {static_cast<float>(window_size.first), static_cast<float>(window_size.second)};

    shader->setUniform(uniform_name_, size);
}

std::shared_ptr<WindowSize> WindowSize::createFromJson(nlohmann::json j) {
    auto uniform = std::make_shared<WindowSize>(j["name"]);

    return uniform;
}

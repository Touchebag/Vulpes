#include "mouse.h"

#include "system/system.h"

#include "utils/log.h"

Mouse::Mouse(sf::RenderWindow& window) :
    window_(window) {
}

void Mouse::saveMousePosition() {
    sf::Vector2i tmp_pos = sf::Mouse::getPosition(window_);
    mouse_pos_ = {tmp_pos.x, tmp_pos.y};

    mouse_world_pos_ = getMouseWorldPosition();
}

std::pair<int, int> Mouse::getMousePosition() {
    sf::Vector2i tmp_pos = sf::Mouse::getPosition(window_);

    return {tmp_pos.x, tmp_pos.y};
}

std::pair<float, float> Mouse::getMouseWorldPosition() {
    auto pixel_pos = sf::Mouse::getPosition(window_);
    auto window_size = window_.getSize();
    auto view = System::getCamera()->getRawView();

    std::pair<float, float> ratio = {
        static_cast<float>(pixel_pos.x) / static_cast<float>(window_size.x),
        static_cast<float>(pixel_pos.y) / static_cast<float>(window_size.y)
    };

    return {
        view.x_pos - (view.width / 2.0) + (view.width * ratio.first),
        view.y_pos - (view.height / 2.0) + (view.height * ratio.second)
    };
}

std::pair<int, int> Mouse::getMouseDistance() {
    sf::Vector2i tmp_pos = sf::Mouse::getPosition(window_);
    std::pair<int, int> mouse_distance = {tmp_pos.x - mouse_pos_.first, tmp_pos.y - mouse_pos_.second};

    return {mouse_distance};
}

std::pair<float, float> Mouse::getMouseWorldDistance() {
    std::pair<float, float> tmp_world_pos = getMouseWorldPosition();

    std::pair<float, float> mouse_world_distance = {tmp_world_pos.first - mouse_world_pos_.first, tmp_world_pos.second - mouse_world_pos_.second};

    return {mouse_world_distance};
}

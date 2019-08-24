#include "mouse.h"

#include "log.h"

Mouse::Mouse(sf::RenderWindow& window) :
    window_(window) {
}

void Mouse::saveMousePosition() {
    sf::Vector2i tmp_pos = sf::Mouse::getPosition(window_);
    mouse_pos_ = {tmp_pos.x, tmp_pos.y};

    sf::Vector2f tmp_world_pos = window_.mapPixelToCoords(tmp_pos);
    mouse_world_pos_ = {tmp_world_pos.x, tmp_world_pos.y};
}


std::pair<int, int> Mouse::getMousePosition() {
    sf::Vector2i tmp_pos = sf::Mouse::getPosition(window_);

    return {tmp_pos.x, tmp_pos.y};
}

std::pair<float, float> Mouse::getMouseWorldPosition() {
    sf::Vector2i tmp_pos = sf::Mouse::getPosition(window_);
    sf::Vector2f tmp_world_pos = window_.mapPixelToCoords(tmp_pos);

    return {tmp_world_pos.x, tmp_world_pos.y};
}

std::pair<int, int> Mouse::getMouseDistance() {
    sf::Vector2i tmp_pos = sf::Mouse::getPosition(window_);
    std::pair<int, int> mouse_distance = {tmp_pos.x - mouse_pos_.first, tmp_pos.y - mouse_pos_.second};

    return {mouse_distance};
}

std::pair<float, float> Mouse::getMouseWorldDistance() {
    sf::Vector2i tmp_pos = sf::Mouse::getPosition(window_);

    sf::Vector2f tmp_world_pos = window_.mapPixelToCoords(tmp_pos);
    std::pair<float, float> mouse_world_distance = {tmp_world_pos.x - mouse_world_pos_.first, tmp_world_pos.y - mouse_world_pos_.second};

    return {mouse_world_distance};
}

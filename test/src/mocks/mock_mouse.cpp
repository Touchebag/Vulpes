#include "editor/common/mouse.h"
#include "mock_mouse.h"

std::pair<int, int> MockMouse::mock_mouse_pos_;
std::pair<float, float> MockMouse::mock_mouse_world_pos_;

// Mock
void MockMouse::setMousePosition(std::pair<int, int> position) {
    mock_mouse_pos_ = position;
}

void MockMouse::setMouseWorldPosition(std::pair<float, float> position) {
    mock_mouse_world_pos_ = position;
}

std::pair<int, int> MockMouse::getMousePosition() {
    return mock_mouse_pos_;
}

std::pair<float, float> MockMouse::getMouseWorldPosition() {
    return mock_mouse_world_pos_;
}

Mouse::Mouse(sf::RenderWindow& window) :
    window_(window) {
    (void)window_;
}

// Class
void Mouse::saveMousePosition() {
    mouse_pos_ = MockMouse::getMousePosition();
    mouse_world_pos_ = MockMouse::getMouseWorldPosition();
}


std::pair<int, int> Mouse::getMousePosition() {
    return MockMouse::getMousePosition();
}

std::pair<float, float> Mouse::getMouseWorldPosition() {
    return MockMouse::getMouseWorldPosition();
}

std::pair<int, int> Mouse::getMouseDistance() {
    auto mock_mouse_pos = MockMouse::getMousePosition();
    sf::Vector2i tmp_pos = {mock_mouse_pos.first, mock_mouse_pos.second};
    std::pair<int, int> mouse_distance = {tmp_pos.x - mouse_pos_.first, tmp_pos.y - mouse_pos_.second};

    return {mouse_distance};
}

std::pair<float, float> Mouse::getMouseWorldDistance() {
    auto mock_mouse_world_pos = MockMouse::getMouseWorldPosition();
    sf::Vector2f tmp_world_pos = {mock_mouse_world_pos.first, mock_mouse_world_pos.second};
    std::pair<float, float> mouse_world_distance = {tmp_world_pos.x - mouse_world_pos_.first, tmp_world_pos.y - mouse_world_pos_.second};

    return {mouse_world_distance};
}

#include "state_object.h"

void StateObject::render(sf::RenderWindow& window) {
    rect_.setPosition(position_);
    window.draw(rect_);
}

bool StateObject::isMouseOver(sf::Vector2f pos) {
    return (pos.x >= position_.x && pos.x <= (position_.x + RECT_SIDE) &&
            pos.y >= position_.y && pos.y <= (position_.y + RECT_SIDE));
}

void StateObject::move(sf::Vector2f pos) {
    position_.x = pos.x - (RECT_SIDE / 2);
    position_.y = pos.y - (RECT_SIDE / 2);
}

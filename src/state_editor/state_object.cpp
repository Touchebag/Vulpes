#include "state_object.h"

#include "log.h"

StateObject::StateObject(nlohmann::json state, sf::Font& font) :
    state_(state),
    font_(font) {
}

void StateObject::render(sf::RenderWindow& window) {
    rect_.setPosition(position_);
    window.draw(rect_);

    sf::Text text;
    text.setFont(font_);
    text.setString(std::to_string(state_["id"].get<int>()));
    text.setCharacterSize(20);
    text.setFillColor(sf::Color::Black);
    text.setPosition(position_.x, position_.y);
    window.draw(text);
}

bool StateObject::isMouseOver(sf::Vector2f pos) {
    return (pos.x >= position_.x && pos.x <= (position_.x + RECT_SIDE) &&
            pos.y >= position_.y && pos.y <= (position_.y + RECT_SIDE));
}

void StateObject::move(sf::Vector2f pos) {
    position_.x = pos.x - (RECT_SIDE / 2);
    position_.y = pos.y - (RECT_SIDE / 2);
}

void StateObject::renderStateText(sf::RenderWindow& window) {
    int i = 0;

    for (auto it = state_.begin(); it != state_.end(); ++it) {
        sf::Text text;
        text.setFont(font_);
        text.setString(it.key());
        text.setCharacterSize(24);
        text.setFillColor(sf::Color::Red);
        text.setPosition(800.0, 100.0 + (i * 30));
        window.draw(text);

        i++;
    }

}

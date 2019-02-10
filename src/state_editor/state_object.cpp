#include <algorithm>

#include "state_object.h"
#include "state_list.h"

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

    renderStateTransitions(window);
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

    sf::Text text;
    text.setFont(font_);
    text.setCharacterSize(14);
    text.setFillColor(sf::Color::Red);

    for (auto it = state_.begin(); it != state_.end(); ++it) {
        text.setString(it.key());
        text.setPosition(50.0, 650.0 + (i * 40));
        window.draw(text);

        text.setString(it.value().dump());
        text.setPosition(100.0, 650.0 + (i * 40) + 20);
        window.draw(text);

        i++;
    }

}

void StateObject::renderStateTransitions(sf::RenderWindow& window) {
    sf::Text text;
    text.setFont(font_);
    text.setCharacterSize(20);
    text.setFillColor(sf::Color::Red);

    std::vector<StateObject>& objects = StateList::getInstance().getObjects();

    for (auto it : state_["next_states"]) {
        int new_state = it["state"].get<int>();
        auto stateObj = std::find_if(objects.begin(),
                                objects.end(),
                                [new_state] (StateObject s) {
                                    return s.state_["id"].get<int>() == new_state;
                                });

        sf::Vertex line[] =
        {
            sf::Vertex(position_),
            sf::Vertex(stateObj->position_)
        };

        window.draw(line, 2, sf::Lines);

        text.setString(std::to_string(it["event"].get<int>()));
        text.setPosition((position_.x + stateObj->position_.x) / 2, (position_.y + stateObj->position_.y) / 2);
        window.draw(text);
    }
}

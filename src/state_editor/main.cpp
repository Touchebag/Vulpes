#include <SFML/Graphics.hpp>

#include "state_object.h"
#include "file.h"
#include "state_list.h"
#include "log.h"

StateObject* current_object = nullptr;

int main() {
    sf::RenderWindow window(sf::VideoMode(1000,1000), "State editor");
    window.setKeyRepeatEnabled(false);

    auto j = file::loadJson("assets/player_state.json");

    sf::Font font;
    if (!font.loadFromFile("assets/arial.ttf")) {
        LOGE("Cannot find font");
        exit(1);
    }

    std::vector<StateObject>& objects = StateList::getInstance().getObjects();

    // TODO Error handling
    if (j) {
        for (auto state : j.value()) {
            objects.push_back({state, font});
        }
    }

    while (window.isOpen()) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)){
            window.close();
        }

        sf::Event event;
        while (window.pollEvent(event)) {
            switch (event.type) {
                case sf::Event::Closed:
                    window.close();
                    break;
                case sf::Event::KeyPressed:
                    LOGV("Key pressed %i\n", event.key.code);
                    break;
                case sf::Event::KeyReleased:
                    LOGV("Key released %i\n", event.key.code);
                    break;
                case sf::Event::MouseButtonPressed:
                    for (int i = 0; static_cast<unsigned int>(i) < objects.size(); ++i) {
                        if (objects.at(i).isMouseOver({static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y)})) {
                            current_object = &objects.at(i);
                            break;
                        }

                        current_object = nullptr;
                    }
                default:
                    LOGV("Unknown event %i\n", event.type);
                    break;
            }
        }

        sf::Vector2i pos = sf::Mouse::getPosition(window);

        window.clear();

        if (current_object) {
            // Move objects around with left mouse
            if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
                current_object->move({static_cast<float>(pos.x), static_cast<float>(pos.y)});
            }

            current_object->renderStateText(window);
        }

        for (auto it : objects) {
            it.render(window);
        }

        window.display();
    }
}

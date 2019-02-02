#include <SFML/Graphics.hpp>

#include "state_object.h"
#include "file.h"
#include "log.h"

std::vector<StateObject> objects;
int current_object = -1;

int main() {
    sf::RenderWindow window(sf::VideoMode(1000,1000), "State editor");
    window.setKeyRepeatEnabled(false);

    auto j = file::loadJson("assets/world.json");

    // TODO Error handling
    if (j) {
        for (auto state : j.value()) {
            objects.push_back({State::loadStateFromJson(state)});
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
                default:
                    LOGV("Unknown event %i\n", event.type);
                    break;
            }
        }

        sf::Vector2i pos = sf::Mouse::getPosition(window);

        // Move objects around with left mouse
        if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
            for (int i = 0; static_cast<unsigned int>(i) < objects.size(); ++i) {
                if (objects.at(i).isMouseOver({static_cast<float>(pos.x), static_cast<float>(pos.y)})) {
                    current_object = i;
                    break;
                }

                current_object = -1;
            }

            if (current_object >= 0) {
                objects.at(current_object).move({static_cast<float>(pos.x), static_cast<float>(pos.y)});
            }
        }

        window.clear();

        for (auto it : objects) {
            it.render(window);
        }

        window.display();
    }
}

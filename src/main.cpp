#include <SFML/Graphics.hpp>

#include "base_entity.h"

int main() {
    sf::RenderWindow window(sf::VideoMode(1000,1000), "BLAAAAH");

    std::vector<BaseEntity> entities;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            entities.push_back(BaseEntity());
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            entities.pop_back();
        }

        window.clear();

        for (auto it = entities.begin(); it != entities.end(); ++it) {
            it->update();
            window.draw(it->sprite_);
        }

        window.display();
    }
}

#include <SFML/Graphics.hpp>

int main() {
    sf::RenderWindow window(sf::VideoMode(1000,1000), "BLAAAAH");

    sf::Texture texture;
    if (!texture.loadFromFile("box.png")) {
        exit(1);
    }

    sf::Sprite sprite;
    sprite.setTexture(texture);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            sprite.move(-0.1, 0);
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            sprite.move(0.1, 0);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
            sprite.move(0, -0.1);
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
            sprite.move(0, 0.1);
        }

        window.clear();
        window.draw(sprite);
        window.display();
    }
}

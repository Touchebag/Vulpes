#include <memory>

#include <SFML/Graphics.hpp>

#include "base_entity.h"
#include "player.h"

int main() {
    sf::RenderWindow window(sf::VideoMode(1000,1000), "BLAAAAH");

    std::vector<std::shared_ptr<BaseEntity>> entities;

    std::shared_ptr<BaseEntity> be1 = std::make_shared<BaseEntity>();
    std::shared_ptr<BaseEntity> be2 = std::make_shared<BaseEntity>();
    std::shared_ptr<BaseEntity> be3 = std::make_shared<BaseEntity>();

    be1->setPosiition(100.0, 200.0);
    entities.push_back(be1);
    be2->setPosiition(400.0, 200.0);
    entities.push_back(be2);
    be3->setPosiition(700.0, 100.0);
    entities.push_back(be3);

    std::shared_ptr<Player> player = std::make_shared<Player>();
    entities.push_back(player);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        window.clear();

        for (auto it = entities.begin(); it != entities.end(); ++it) {
            (*it)->update();
            (*it)->render(window);
        }

        window.display();
    }
}

#include <memory>

#include <SFML/Graphics.hpp>

#include "base_entity.h"
#include "player.h"
#include "world.h"

int main() {
    sf::RenderWindow window(sf::VideoMode(1000,1000), "BLAAAAH");

    std::vector<std::shared_ptr<BaseEntity>> entities;

    World& worldInst = World::getInstance();

    sf::Time frames;
    sf::Clock frame_time;

    std::shared_ptr<BaseEntity> be1 = std::make_shared<BaseEntity>();
    std::shared_ptr<BaseEntity> be2 = std::make_shared<BaseEntity>();
    std::shared_ptr<BaseEntity> be3 = std::make_shared<BaseEntity>();

    be1->setPosiition(100.0, 200.0);
    be1->setHitbox(25.0, -25.0, -25.0, 25.0);
    worldInst.getWorldObjects().push_back(be1);
    be2->setPosiition(400.0, 200.0);
    be2->setHitbox(10.0, -30.0, -20.0, 70.0);
    worldInst.getWorldObjects().push_back(be2);
    be3->setPosiition(700.0, 100.0);
    be3->setHitbox(34.0, -56.0, -13.0, 43.0);
    worldInst.getWorldObjects().push_back(be3);

    std::shared_ptr<Player> player = std::make_shared<Player>();
    player->setHitbox(50.0, -50.0, -50.0, 50.0);
    entities.push_back(player);

    frame_time.restart();
    while (window.isOpen()) {
        frames += frame_time.getElapsedTime();
        frame_time.restart();

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)){
            window.close();
        }

        window.clear();

        // If we have rendered more than one physics frame then advance physics
        while (frames.asMilliseconds() >= 16) {
            for (auto it = entities.begin(); it != entities.end(); ++it) {
                (*it)->update();
            }

            for (auto it = worldInst.getWorldObjects().begin(); it != worldInst.getWorldObjects().end(); ++it) {
                (*it)->update();
            }

            frames -= sf::milliseconds(16);
        }

        for (auto it = entities.begin(); it != entities.end(); ++it) {
            (*it)->render(window);
        }

        for (auto it = worldInst.getWorldObjects().begin(); it != worldInst.getWorldObjects().end(); ++it) {
            (*it)->render(window);
        }

        window.display();
    }
}

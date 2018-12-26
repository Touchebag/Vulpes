#include <memory>

#include <SFML/Graphics.hpp>
#include <json.hpp>
#include <fstream>

#include "base_entity.h"
#include "player.h"
#include "world.h"

#define PHYSICS_FRAME_RATE 60

int main() {
    sf::RenderWindow window(sf::VideoMode(1000,1000), "BLAAAAH");

    std::vector<std::shared_ptr<BaseEntity>> entities;

    World& worldInst = World::getInstance();

    sf::Time frames;
    sf::Clock frame_time;

    std::ifstream ifs("assets/world.json");
    nlohmann::json j = nlohmann::json::parse(ifs);

    for (auto it : j) {
        BaseEntity ent;
        ent.loadFromJson(it);
        std::shared_ptr<BaseEntity> entity = std::make_shared<BaseEntity>(ent);
        worldInst.getWorldObjects().push_back(std::move(entity));
    }

    std::shared_ptr<Player> player = std::make_shared<Player>();
    player->setPosiition(200.0, 200.0);
    player->setHitbox(25.0, -25.0, -25.0, 25.0);
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
        while (frames.asMilliseconds() >= (1000.0 / PHYSICS_FRAME_RATE)) {
            for (auto it = entities.begin(); it != entities.end(); ++it) {
                (*it)->update();
            }

            for (auto it = worldInst.getWorldObjects().begin(); it != worldInst.getWorldObjects().end(); ++it) {
                (*it)->update();
            }

            frames -= sf::milliseconds(1000.0 / PHYSICS_FRAME_RATE);
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

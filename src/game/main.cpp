#include <memory>

#include <SFML/Graphics.hpp>
#include <json.hpp>

#include "base_entity.h"
#include "player.h"
#include "world.h"
#include "utils/common.h"
#include "file.h"
#include "log.h"
#include "input_event.h"

#define PHYSICS_FRAME_RATE 60

int main() {
    sf::RenderWindow window(sf::VideoMode(1000,1000), "BLAAAAH");
    window.setKeyRepeatEnabled(false);

    std::vector<std::shared_ptr<BaseEntity>> entities;

    World& worldInst = World::getInstance();

    sf::Time frames;
    sf::Clock frame_time;

    std::optional<nlohmann::json> j = file::loadJson("assets/world.json");

    if (j) {
        for (auto it : j.value()) {
            std::shared_ptr<BaseEntity> ent = std::make_shared<BaseEntity>();
            ent->loadFromJson(it);
            worldInst.getWorldObjects().push_back(std::move(ent));
        }
    }

    Input::getInstance().setKeyboardMap(
            {{sf::Keyboard::Key::Space, input::button::JUMP},
            {sf::Keyboard::Key::Left, input::button::LEFT},
            {sf::Keyboard::Key::Right, input::button::RIGHT}});

    std::shared_ptr<Player> player = std::make_shared<Player>();
    player->setPosiition(util::X(200.0), util::Y(200.0));
    player->setHitbox(util::Right(25.0), util::Left(-25.0), util::Top(-100.0), util::Bottom(100.0));
    player->loadTexture("Player.png");
    player->load_sprite_map("Player.txt");
    entities.push_back(player);

    frame_time.restart();
    while (window.isOpen()) {
        frames += frame_time.getElapsedTime();
        frame_time.restart();

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)){
            window.close();
        }

        window.clear();

        // If we have rendered more than one physics frame then advance physics
        while (frames.asMilliseconds() >= (1000.0 / PHYSICS_FRAME_RATE)) {
            Input::getInstance().update();

            sf::Event event;
            while (window.pollEvent(event)) {
                switch (event.type) {
                    case sf::Event::Closed:
                        window.close();
                        break;
                    case sf::Event::KeyPressed:
                        LOGV("Key pressed %i", event.key.code);
                        Input::getInstance().keyEvent(event.key.code, true);
                        break;
                    case sf::Event::KeyReleased:
                        LOGV("Key released %i", event.key.code);
                        Input::getInstance().keyEvent(event.key.code, false);
                        break;
                    default:
                        LOGV("Unknown event %i", event.type);
                        break;
                }
            }

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

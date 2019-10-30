#include <memory>

#include <json.hpp>

#include "base_entity.h"
#include "world.h"
#include "utils/common.h"
#include "file.h"
#include "log.h"
#include "input_event.h"
#include "render.h"

#define PHYSICS_FRAME_RATE 60
#define MS_PER_FRAME 1000 / PHYSICS_FRAME_RATE

int game_main(sf::RenderWindow& window, std::string level_file_path) {
    World& worldInst = World::getInstance();

    sf::Time frames;
    sf::Clock frame_time;

    worldInst.loadWorldFromFile(level_file_path);

    Input::getInstance().setKeyboardMap(
            {{sf::Keyboard::Key::Space, Actions::Action::JUMP},
            {sf::Keyboard::Key::Left, Actions::Action::MOVE_LEFT},
            {sf::Keyboard::Key::Right, Actions::Action::MOVE_RIGHT},
            {sf::Keyboard::Key::LShift, Actions::Action::DASH}});

    frame_time.restart();

    while (window.isOpen()) {
        frames += frame_time.getElapsedTime();
        frame_time.restart();

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)){
            window.close();
        }

        window.clear();

        // If we have rendered more than one physics frame then advance physics
        while (frames.asMilliseconds() >= (MS_PER_FRAME)) {
            sf::Event event;
            while (window.pollEvent(event)) {
                switch (event.type) {
                    case sf::Event::Closed:
                        window.close();
                        break;
                    case sf::Event::KeyPressed:
                        if (event.key.code == sf::Keyboard::Key::P) {
                            return 0;
                        }
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

            worldInst.update();

            frames -= sf::milliseconds(MS_PER_FRAME);
        }

        Render& renderInst = Render::getInstance();

        auto view_pos = worldInst.getPlayerPosition();
        renderInst.setView(static_cast<float>(view_pos.x), static_cast<float>(view_pos.y), 1000.0, 1000.0);

        renderInst.render(window);

        window.display();
    }

    return 0;
}

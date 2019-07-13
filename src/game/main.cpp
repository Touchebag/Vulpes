#include <memory>

#include <SFML/Graphics.hpp>
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

int main() {
    sf::RenderWindow window(sf::VideoMode(1000,1000), "BLAAAAH");
    window.setKeyRepeatEnabled(false);

    World& worldInst = World::getInstance();

    sf::Time frames;
    sf::Clock frame_time;

    worldInst.loadWorld("assets/world.json");

    Input::getInstance().setKeyboardMap(
            {{sf::Keyboard::Key::Space, input::button::JUMP},
            {sf::Keyboard::Key::Left, input::button::LEFT},
            {sf::Keyboard::Key::Right, input::button::RIGHT},
            {sf::Keyboard::Key::LShift, input::button::DASH}});

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

            worldInst.update();

            frames -= sf::milliseconds(MS_PER_FRAME);
        }

        Render& renderInst = Render::getInstance();

        auto view_pos = worldInst.getPlayerPosition();
        renderInst.setView(static_cast<float>(view_pos.x), static_cast<float>(view_pos.y), 1000.0, 1000.0);

        renderInst.render(window);

        window.display();
    }
}

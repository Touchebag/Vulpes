#include <memory>

#include "nlohmann/json.hpp"

#include "base_entity.h"
#include "system/system.h"
#include "utils/common.h"
#include "utils/log.h"

#define PHYSICS_FRAME_RATE 60
#define MS_PER_FRAME 1000 / PHYSICS_FRAME_RATE

int gameMain(sf::RenderWindow& window) {
    // Rendering produces time, physics consumes
    // This stores how much "unconsumed" time is available
    sf::Time time_rendered;
    sf::Clock render_clock;

    System::getInput()->setKeyboardMap({
            {sf::Keyboard::Key::Space, {Actions::Action::JUMP,
                                        Actions::Action::HIGH_JUMP}},
            {sf::Keyboard::Key::Left, {Actions::Action::MOVE_LEFT}},
            {sf::Keyboard::Key::Right, {Actions::Action::MOVE_RIGHT}},
            {sf::Keyboard::Key::Up, {Actions::Action::MOVE_UP,
                                     Actions::Action::INTERACT}},
            {sf::Keyboard::Key::Down, {Actions::Action::MOVE_DOWN,
                                       Actions::Action::AIR_DIVE}},
            {sf::Keyboard::Key::LShift, {Actions::Action::DASH}},
            {sf::Keyboard::Key::Z, {Actions::Action::ATTACK1}},
            {sf::Keyboard::Key::C, {Actions::Action::ATTACK2}}
            });

    System::getInput()->setButtonMap({
            {0, {Actions::Action::JUMP,
                 Actions::Action::HIGH_JUMP}},
            {5, {Actions::Action::DASH}},
            {2, {Actions::Action::ATTACK1}},
            {1, {Actions::Action::INTERACT}}
            });

    render_clock.restart();

    while (window.isOpen()) {
        time_rendered += render_clock.getElapsedTime();
        render_clock.restart();

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)){
            File().writeSaveFile(System::getEnvironment()->outputEnvToJson());
            window.close();
        }

        // If we have rendered more than one physics frame then advance physics
        while (time_rendered.asMilliseconds() >= (MS_PER_FRAME)) {
            System::getInput()->update(window);

            sf::Event event;
            while (window.pollEvent(event)) {
                switch (event.type) {
                    case sf::Event::Closed:
                        window.close();
                        break;
                    case sf::Event::Resized:
                        System::getRender()->setWindowSize(window, event.size.width, event.size.height);
                        break;
                    case sf::Event::KeyPressed:
                        if (event.key.code == sf::Keyboard::Key::P) {
                            return 0;
                        }
                        break;
                    default:
                        LOGV("Unknown event %i", event.type);
                        break;
                }
            }

            System::IWorldModify().update();

            time_rendered -= sf::milliseconds(MS_PER_FRAME);

            System::getCamera()->update();
        }

        window.clear();

        System::getRender()->render(window, static_cast<float>(time_rendered.asMilliseconds()) / static_cast<float>(MS_PER_FRAME));

        window.display();
    }

    return 0;
}

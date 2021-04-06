#include <memory>

#include <json.hpp>

#include "base_entity.h"
#include "system/system.h"
#include "utils/common.h"
#include "utils/log.h"
#include "system/input_event.h"

#define PHYSICS_FRAME_RATE 60
#define MS_PER_FRAME 1000 / PHYSICS_FRAME_RATE

int game_main(sf::RenderWindow& window) {
    auto worldInstWrite = System::IWorldModify();

    // Rendering produces time, physics consumes
    // This stores how much "unconsumed" time is available
    sf::Time time_rendered;
    sf::Clock render_clock;

    Input::getInstance().setKeyboardMap({
            {sf::Keyboard::Key::Space, {Actions::Action::JUMP,
                                        Actions::Action::WALL_JUMP,
                                        Actions::Action::DOUBLE_JUMP}},
            {sf::Keyboard::Key::Left, {Actions::Action::MOVE_LEFT}},
            {sf::Keyboard::Key::Right, {Actions::Action::MOVE_RIGHT}},
            {sf::Keyboard::Key::Down, {Actions::Action::AIR_DIVE}},
            {sf::Keyboard::Key::LShift, {Actions::Action::DASH}},
            {sf::Keyboard::Key::Z, {Actions::Action::ATTACK}},
            {sf::Keyboard::Key::Up, {Actions::Action::INTERACT}}
            });

    Input::getInstance().setButtonMap({
            {0, {Actions::Action::JUMP,
                 Actions::Action::WALL_JUMP,
                 Actions::Action::DOUBLE_JUMP}},
            {5, {Actions::Action::DASH}},
            {2, {Actions::Action::ATTACK}},
            {1, {Actions::Action::INTERACT}}
            });

    render_clock.restart();

    std::shared_ptr<IRender> renderInst = System::getRender();

    while (window.isOpen()) {
        time_rendered += render_clock.getElapsedTime();
        render_clock.restart();

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)){
            window.close();
        }

        // If we have rendered more than one physics frame then advance physics
        while (time_rendered.asMilliseconds() >= (MS_PER_FRAME)) {
            Input::getInstance().update(window);

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

            worldInstWrite.update();

            time_rendered -= sf::milliseconds(MS_PER_FRAME);

            System::getCamera()->update();
        }

        window.clear();

        renderInst->render(window, static_cast<float>(time_rendered.asMilliseconds()) / static_cast<float>(MS_PER_FRAME));

        window.display();
    }

    return 0;
}

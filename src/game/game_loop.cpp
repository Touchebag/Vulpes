#include <memory>

#include <json.hpp>

#include "base_entity.h"
#include "system/world.h"
#include "utils/common.h"
#include "utils/log.h"
#include "system/input_event.h"
#include "system/system.h"

#define PHYSICS_FRAME_RATE 60
#define MS_PER_FRAME 1000 / PHYSICS_FRAME_RATE

#define VIEW_SIZE_SCALING 1.0f

int game_main(sf::RenderWindow& window) {
    auto worldInstWrite = World::getInstance<World::IWorldModify>();

    sf::Time frames;
    sf::Clock frame_time;

    Input::getInstance().setKeyboardMap({
            {sf::Keyboard::Key::Space, {Actions::Action::JUMP,
                                        Actions::Action::WALL_JUMP,
                                        Actions::Action::DOUBLE_JUMP}},
            {sf::Keyboard::Key::Left, {Actions::Action::MOVE_LEFT}},
            {sf::Keyboard::Key::Right, {Actions::Action::MOVE_RIGHT}},
            {sf::Keyboard::Key::LShift, {Actions::Action::DASH}},
            {sf::Keyboard::Key::Z, {Actions::Action::ATTACK}},
            {sf::Keyboard::Key::Up, {Actions::Action::INTERACT}}
            });

    frame_time.restart();

    std::shared_ptr<IRender> renderInst = System::getRender();

    {
        auto view_size = window.getView().getSize();
        System::getCamera()->setView(0, 0, view_size.x * VIEW_SIZE_SCALING, view_size.y * VIEW_SIZE_SCALING);
    }

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
                    case sf::Event::Resized:
                        System::getCamera()->resizeView(static_cast<float>(event.size.width) * VIEW_SIZE_SCALING, static_cast<float>(event.size.height) * VIEW_SIZE_SCALING);
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

            worldInstWrite.update();

            frames -= sf::milliseconds(MS_PER_FRAME);
        }

        System::getCamera()->update();

        renderInst->render(window);

        window.display();
    }

    return 0;
}

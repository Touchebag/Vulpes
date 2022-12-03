#include "cutscene_editor.h"

#include "system/system.h"
#include "base_entity.h"
#include "editor_render.h"

#include "utils/log.h"

#define PHYSICS_FRAME_RATE 60
#define MS_PER_FRAME 1000 / PHYSICS_FRAME_RATE

int cutsceneEditorMain(sf::RenderWindow& window) {
    auto old_render = System::getRender();
    auto old_camera = System::getCamera();
    auto old_world = System::getWorld();
    auto render = std::make_shared<Render>();
    auto camera = std::make_shared<Camera>();
    auto world = std::make_shared<World>();

    camera->setView(0, 0, 1000, 1000);

    System::setRender(render);
    System::setCamera(camera);
    System::setWorld(world);

    render->setWindowSize(window, 1000, 1000);

    window.setKeyRepeatEnabled(false);

    auto cutscene = Cutscene::loadCutscene("boxes");

    System::setCutscene(cutscene);

    while (window.isOpen()) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape) && window.hasFocus()){
            break;
        }

        sf::Event event;
        while (window.pollEvent(event)) {
            switch (event.type) {
                case sf::Event::KeyPressed:
                    if (event.key.code == sf::Keyboard::Key::Right) {
                        System::IWorldModify::update();
                    }
                    break;
                default:
                    break;
            }
        }

        window.clear();
        System::getRender()->render(window, 0.0);
        window.display();
    }

    System::setRender(old_render);
    System::setCamera(old_camera);
    System::setWorld(old_world);

    return 0;
}

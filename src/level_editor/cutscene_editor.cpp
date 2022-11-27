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
    render->setCameraBox({0, 0, 1000, 1000});

    window.setKeyRepeatEnabled(false);

    auto cutscene = Cutscene::loadCutscene("door_transition");

    System::setCutscene(cutscene);

    sf::Time time_rendered;
    sf::Clock render_clock;
    render_clock.restart();

    while (window.isOpen()) {
        time_rendered += render_clock.getElapsedTime();
        render_clock.restart();

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape) && window.hasFocus()){
            break;
        }

        System::IWorldModify::update();

        while (time_rendered.asMilliseconds() >= (MS_PER_FRAME)) {
            System::IWorldModify::update();

            time_rendered -= sf::milliseconds(MS_PER_FRAME);
        }

        window.clear();
        render->render(window, 0.0);
        window.display();
    }

    System::setRender(old_render);
    System::setCamera(old_camera);
    System::setWorld(old_world);

    return 0;
}

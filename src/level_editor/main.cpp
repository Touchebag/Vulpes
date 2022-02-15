#include "game_loop.h"
#include "editor_loop.h"
#include "system/system.h"
#include "system/world.h"
#include "editor_render.h"

int main(int argc, const char** argv) {
    sf::RenderWindow window(sf::VideoMode(1000,1000), "Level Editor");

    System::setRender(std::make_shared<EditorRender>());
    System::getRender()->setWindowSize(window, 1000, 1000);

    window.setKeyRepeatEnabled(false);

    std::string level_file;
    if (argc > 1) {
        level_file = argv[1];
    } else {
        level_file = "world.json";
    }

    System::IWorldModify::loadWorldFromFile(level_file);
    System::IWorldModify::setEntrance(0);

    while (window.isOpen()) {
        level_editor_main(window);

        if (!window.isOpen()) {
            break;
        }

        game_main(window);
    }
}

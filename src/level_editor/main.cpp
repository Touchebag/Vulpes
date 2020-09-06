#include "game_loop.h"
#include "editor_loop.h"
#include "system/system.h"
#include "system/world.h"
#include "editor_render.h"

int main(int argc, char** argv) {
    sf::RenderWindow window(sf::VideoMode(1000,1000), "Level Editor");
    window.setKeyRepeatEnabled(false);

    std::string level_file;
    if (argc > 1) {
        level_file = argv[1];
    } else {
        level_file = "world.json";
    }

    System::setRender(std::make_shared<EditorRender>());

    World::IWorldModify::loadWorldFromFile(level_file);
    World::IWorldModify::setEntrance(0);

    while (window.isOpen()) {
        game_main(window);

        if (!window.isOpen()) {
            break;
        }

        level_editor_main(window);
    }
}

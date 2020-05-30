#include "game_loop.h"
#include "editor_loop.h"

int main(int argc, char** argv) {
    sf::RenderWindow window(sf::VideoMode(1000,1000), "Level Editor");
    window.setKeyRepeatEnabled(false);

    std::string level_file;
    if (argc > 1) {
        level_file = argv[1];
    } else {
        level_file = "world.json";
    }

    while (window.isOpen()) {
        game_main(window, level_file);

        if (!window.isOpen()) {
            break;
        }

        level_editor_main(window, level_file);
    }
}

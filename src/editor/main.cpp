#include "game_loop.h"
#include "system/system.h"
#include "system/world.h"
#include "common/editor_render.h"
#include "common/common.h"

#include "level_editor/level_editor_main.h"
#include "cutscene_editor/cutscene_editor_main.h"
#include "entity_editor/entity_editor_main.h"

#include <imgui.h>
#include <imgui-SFML.h>

#include "utils/log.h"

using editor_common::CurrentEditor;

int main(int argc, const char** argv) {
    sf::RenderWindow window(sf::VideoMode(1000,1000), "Level Editor");

    System::setRender(std::make_shared<EditorRender>());
    System::getRender()->setWindowSize(window, 1000, 1000);

    window.setKeyRepeatEnabled(false);

    if (!ImGui::SFML::Init(window)) {
        throw std::runtime_error("Failed to initialise ImGui, exiting");
    }

    std::string level_file;
    if (argc > 1) {
        level_file = argv[1];
    } else {
        level_file = "world.json";
    }

    System::IWorldModify::loadWorldFromFile(level_file);
    System::IWorldModify::setEntrance(0);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            switch (event.type) {
                case sf::Event::KeyPressed:
                    if (event.key.code == sf::Keyboard::Key::Escape) {
                        window.close();
                    }
                    break;
                default:
                    break;
            }
        }

        switch (editor_common::getCurrentEditor()) {
            case CurrentEditor::GAME:
                gameMain(window);
                break;
            case CurrentEditor::LEVEL:
                levelEditorMain(window);
                break;
            case CurrentEditor::CUTSCENE:
                cutsceneEditorMain(window);
                break;
            case CurrentEditor::ENTITY:
                entityEditorMain(window);
                break;
            default:
                ImGui::SFML::Shutdown();
                exit(0);
        }
    }
}

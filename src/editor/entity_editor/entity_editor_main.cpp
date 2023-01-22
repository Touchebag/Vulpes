#include "entity_editor_main.h"

#include <imgui.h>
#include <imgui-SFML.h>

#include "system/system.h"
#include "base_entity.h"
#include "common/editor_render.h"
#include "common/common.h"

#include "entity_top_menu.h"
#include "unpacked_entity.h"
#include "editor_view.h"

#include "utils/log.h"

#define PHYSICS_FRAME_RATE 60
#define MS_PER_FRAME 1000 / PHYSICS_FRAME_RATE

int entityEditorMain(sf::RenderWindow& window) {
    sf::Clock delta_clock;

    window.setKeyRepeatEnabled(true);

    entity_editor::EditorView view(window);
    view.setView({500.0, 500.0}, {1000.0, 1000.0});

    auto top_menu = entity_top_menu::createMenus();

    auto entity = UnpackedEntity::unpackEntity("player");

    while (window.isOpen() && editor_common::getCurrentEditor() == editor_common::CurrentEditor::ENTITY) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape) && window.hasFocus()){
            window.close();
        }

        sf::Event event;
        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(event);

            view.handleMouseEvent(event);

            switch (event.type) {
                default:
                    break;
            }
        }

        ImGui::SFML::Update(window, delta_clock.restart());

        window.clear(sf::Color(255, 255, 255));

        view.update();
        top_menu.draw(window);

        entity.update();

        ImGui::SFML::Render(window);

        window.display();
    }

    return 0;
}

#include "entity_editor_main.h"

#include <imgui.h>
#include <imgui-SFML.h>

#include "system/system.h"
#include "base_entity.h"
#include "common/editor_render.h"
#include "common/common.h"
#include "entity_top_menu.h"

#include "unpacked_entity.h"

#include "utils/log.h"

#define PHYSICS_FRAME_RATE 60
#define MS_PER_FRAME 1000 / PHYSICS_FRAME_RATE

void setWindowSize(sf::RenderWindow& window, int width, int height) {
    auto view = window.getView();
    view.setSize({static_cast<float>(width), static_cast<float>(height)});
    view.setCenter({static_cast<float>(width) / 2.0f, static_cast<float>(height) / 2.0f});

    window.setView(view);
}

int entityEditorMain(sf::RenderWindow& window) {
    setWindowSize(window, 1000, 1000);
    sf::Clock delta_clock;

    window.setKeyRepeatEnabled(true);

    auto top_menu = entity_top_menu::createMenus();

    auto entity = UnpackedEntity::unpackEntity("player");

    while (window.isOpen() && editor_common::getCurrentEditor() == editor_common::CurrentEditor::ENTITY) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape) && window.hasFocus()){
            window.close();
        }

        sf::Event event;
        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(event);
        }

        ImGui::SFML::Update(window, delta_clock.restart());

        window.clear(sf::Color(255, 255, 255));
        top_menu.draw(window);

        entity.update(window);

        ImGui::SFML::Render(window);

        window.display();
    }

    return 0;
}

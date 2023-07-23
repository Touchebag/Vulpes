#pragma once

#include <imgui.h>
#include <imgui-SFML.h>

#include <nlohmann/json.hpp>

#include "components/animation_editor.h"
#include "components/state_editor.h"
#include "common/mouse.h"
#include "entity_common.h"

namespace entity_editor {

class UnpackedEntity {
  public:
    static UnpackedEntity unpackEntity(const std::string& entity_name);

    void handleKeyPress(sf::Event event);
    void handleMouseClick(sf::Event event, Mouse mouse);

    void draw(sf::RenderWindow& window);

  private:
    std::string entity_name_;

    entity_editor::AnimationEditor animation_editor_;
    entity_editor::StateEditor state_editor_;
};

} // entity_editor

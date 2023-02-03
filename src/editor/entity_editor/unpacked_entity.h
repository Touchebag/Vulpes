#pragma once

#include <imgui.h>
#include <imgui-SFML.h>

#include <nlohmann/json.hpp>

#include "state_view.h"
#include "common/mouse.h"

class UnpackedEntity {
  public:
    static UnpackedEntity unpackEntity(const std::string& entity_name);

    void handleKeyPress(sf::Event event);
    void handleMouseClick(sf::Event event, Mouse mouse);

    void draw(sf::RenderWindow& window);

  private:
    std::string entity_name_;

    StateView state_view;
};

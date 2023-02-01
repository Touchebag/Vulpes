#pragma once

#include <imgui.h>
#include <imgui-SFML.h>

#include <nlohmann/json.hpp>

#include "state_view.h"

class UnpackedEntity {
  public:
    static UnpackedEntity unpackEntity(const std::string& entity_name);

    void handleMouseClick(std::pair<float, float> mouse_pos);

    void draw(sf::RenderWindow& window);

  private:
    std::string entity_name_;

    StateView state_view;
};
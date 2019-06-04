#pragma once

#include "base_entity.h"

#include <SFML/Graphics.hpp>

enum class render_layer {
    // Counting outwards from MAIN
    BG_3,
    BG_2,
    BG_1,

    MAIN_BG,
    MAIN,
    MAIN_FG,

    // Counting outwards from MAIN
    FG_1,
    FG_2,
    FG_3,

    MAX_LAYERS
};

class Render {
  public:
    void render(sf::RenderWindow& window);

    void addEntity(std::shared_ptr<BaseEntity> entity, render_layer layer);
    void removeEntity(std::shared_ptr<BaseEntity> entity);

    static Render& getInstance();

  private:
    Render() = default;

    std::array<std::vector<std::shared_ptr<BaseEntity>>, static_cast<int>(render_layer::MAX_LAYERS)> layers_;
};

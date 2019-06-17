#pragma once

#include "base_entity.h"

#include <SFML/Graphics.hpp>

class Render {
  public:
    enum class Layer {
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

    void render(sf::RenderWindow& window);

    void addEntity(std::shared_ptr<BaseEntity> entity, Layer layer);
    void removeEntity(std::shared_ptr<BaseEntity> entity);

    void setView(float x, float y, float width, float height);

    static Render& getInstance();

    // Needed for Level Editor
    // TODO Conditiionally compile?
    std::vector<std::shared_ptr<BaseEntity>> getLayer(Layer layer);
    void renderLayer(sf::RenderWindow& window, Layer layer);

  private:
    float view_x_;
    float view_y_;
    float view_width_;
    float view_height_;

    Render() = default;

    std::array<std::vector<std::shared_ptr<BaseEntity>>, static_cast<int>(Layer::MAX_LAYERS)> layers_;
};

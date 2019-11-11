#include "system/render.h"

namespace {

std::map<World::Layer, float> parallax_map = {
    {World::Layer::BACKGROUND, 0.0},
    {World::Layer::BG_3, 0.85},
    {World::Layer::BG_2, 0.9},
    {World::Layer::BG_1, 0.95},
    {World::Layer::MAIN_BG, 1.0},
    {World::Layer::MAIN, 1.0},
    {World::Layer::MAIN_FG, 1.0},
    {World::Layer::FG_1, 1.05},
    {World::Layer::FG_2, 1.1},
    {World::Layer::FG_3, 1.15},
};

void renderAllEntitesInVector(std::vector<std::weak_ptr<RenderableEntity>>& layer, sf::RenderWindow& window) {
    for (auto it = layer.begin(); it != layer.end(); ) {
        if (auto ptr = it->lock()) {
            ptr->render(window);
            ++it;
        } else {
            it = layer.erase(it);
        }
    }
}

}

void Render::renderLayer(sf::RenderWindow& window, World::Layer layer) {
    float parallax_mulitiplier = 1;

    if (parallax_enabled_) {
        parallax_mulitiplier = parallax_map[layer];
    }

    sf::View viewport({view_x_ * parallax_mulitiplier, view_y_ * parallax_mulitiplier}, {view_width_, view_height_});
    window.setView(viewport);

    renderAllEntitesInVector(layers_[static_cast<int>(layer)], window);
}

void Render::setView(float x, float y, float width, float height) {
    view_x_ = x;
    view_y_ = y;
    view_width_ = width;
    view_height_ = height;
}

sf::View Render::getView() {
    return {{view_x_, view_y_}, {view_width_, view_height_}};
}

void Render::drawHud(sf::RenderWindow& window) {
    auto view = window.getView();

    window.setView({{500, 500}, {1000, 1000}});
    renderAllEntitesInVector(hud_layer_, window);

    window.setView(view);
}

void Render::render(sf::RenderWindow& window) {
    for (int i = 0; i < static_cast<int>(World::Layer::MAX_LAYERS); ++i) {
        renderLayer(window, static_cast<World::Layer>(i));
    }

    drawHud(window);
}

void Render::addEntity(std::weak_ptr<RenderableEntity> entity, World::Layer layer) {
    if (layer == World::Layer::HUD) {
        hud_layer_.push_back(entity);
    } else {
        layers_[static_cast<int>(layer)].push_back(entity);
    }
}

Render& Render::getInstance() {
    static Render render_instance;

    return render_instance;
}

#include "render.h"

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

}

void Render::renderLayer(sf::RenderWindow& window, World::Layer layer) {
    float parallax_mulitiplier = 1;

    if (parallax_enabled_) {
        parallax_mulitiplier = parallax_map[layer];
    }

    sf::View viewport({view_x_ * parallax_mulitiplier, view_y_ * parallax_mulitiplier}, {view_width_, view_height_});
    window.setView(viewport);
    for (auto it = layers_[static_cast<int>(layer)].begin(); it != layers_[static_cast<int>(layer)].end(); ++it) {
        (*it)->render(window);
    }
}

void Render::setView(float x, float y, float width, float height) {
    view_x_ = x;
    view_y_ = y;
    view_width_ = width;
    view_height_ = height;
}

void Render::render(sf::RenderWindow& window) {
    for (int i = 0; i < static_cast<int>(World::Layer::MAX_LAYERS); ++i) {
        renderLayer(window, static_cast<World::Layer>(i));
    }
}

void Render::addEntity(std::shared_ptr<BaseEntity> entity, World::Layer layer) {
    layers_[static_cast<int>(layer)].push_back(entity);
}

void Render::removeEntity(std::shared_ptr<BaseEntity> entity, World::Layer layer) {
    auto& layer_list = layers_[static_cast<int>(layer)];
    layer_list.erase(std::remove(layer_list.begin(), layer_list.end(), entity), layer_list.end());
}

Render& Render::getInstance() {
    static Render render_instance;

    return render_instance;
}

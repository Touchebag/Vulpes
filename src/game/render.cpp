#include "render.h"

namespace {

std::map<Render::Layer, float> parallax_map = {
    {Render::Layer::BACKGROUND, 0.0},
    {Render::Layer::BG_3, 0.85},
    {Render::Layer::BG_2, 0.9},
    {Render::Layer::BG_1, 0.95},
    {Render::Layer::MAIN_BG, 1.0},
    {Render::Layer::MAIN, 1.0},
    {Render::Layer::MAIN_FG, 1.0},
    {Render::Layer::FG_1, 1.05},
    {Render::Layer::FG_2, 1.1},
    {Render::Layer::FG_3, 1.15},
};

std::map<Render::Layer, std::string> LayerStringMap = {
    {Render::Layer::BACKGROUND, "background"},
    {Render::Layer::BG_3, "bg3"},
    {Render::Layer::BG_2, "bg2"},
    {Render::Layer::BG_1, "bg1"},
    {Render::Layer::MAIN_BG, "main_bg"},
    {Render::Layer::MAIN, "main"},
    {Render::Layer::MAIN_FG, "main_fg"},
    {Render::Layer::FG_1, "fg1"},
    {Render::Layer::FG_2, "fg2"},
    {Render::Layer::FG_3, "fg3"},
};

}

std::string Render::getLayerString(Layer layer) {
    return LayerStringMap[layer];
}

void Render::renderLayer(sf::RenderWindow& window, Layer layer) {
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

std::vector<std::shared_ptr<BaseEntity>> Render::getLayer(Layer layer) {
    return layers_[static_cast<int>(layer)];
}

void Render::setView(float x, float y, float width, float height) {
    view_x_ = x;
    view_y_ = y;
    view_width_ = width;
    view_height_ = height;
}

void Render::render(sf::RenderWindow& window) {
    for (int i = 0; i < static_cast<int>(Layer::MAX_LAYERS); ++i) {
        renderLayer(window, static_cast<Layer>(i));
    }
}

void Render::addEntity(std::shared_ptr<BaseEntity> entity, Layer layer) {
    layers_[static_cast<int>(layer)].push_back(entity);
}

void Render::removeEntity(std::shared_ptr<BaseEntity> entity, Layer layer) {
    auto& layer_list = layers_[static_cast<int>(layer)];
    layer_list.erase(std::remove(layer_list.begin(), layer_list.end(), entity), layer_list.end());
}

Render& Render::getInstance() {
    static Render render_instance;

    return render_instance;
}

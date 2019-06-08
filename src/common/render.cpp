#include "render.h"

namespace {
std::map<Render::Layer, float> parallax_map = {
    {Render::Layer::MAIN, 1.0}
};
}

void Render::renderLayer(sf::RenderWindow& window, Layer layer) {
    for (auto it = layers_[static_cast<int>(layer)].begin(); it != layers_[static_cast<int>(layer)].end(); ++it) {
        (*it)->render(window);
    }
}

void Render::render(sf::RenderWindow& window) {
    renderLayer(window, Layer::BG_3);
    renderLayer(window, Layer::BG_2);
    renderLayer(window, Layer::BG_1);
    renderLayer(window, Layer::MAIN);
}

void Render::addEntity(std::shared_ptr<BaseEntity> entity, Layer layer) {
    layers_[static_cast<int>(layer)].push_back(entity);
}

void Render::removeEntity(std::shared_ptr<BaseEntity> entity) {
}

Render& Render::getInstance() {
    static Render render_instance;

    return render_instance;
}

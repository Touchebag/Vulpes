#include "render.h"

void Render::render(sf::RenderWindow& window) {
    for (auto it = layers_[static_cast<int>(render_layer::MAIN)].begin(); it != layers_[static_cast<int>(render_layer::MAIN)].end(); ++it) {
        (*it)->render(window);
    }
}

void Render::addEntity(std::shared_ptr<BaseEntity> entity, render_layer layer) {
    layers_[static_cast<int>(layer)].push_back(entity);
}

void Render::removeEntity(std::shared_ptr<BaseEntity> entity) {
}

Render& Render::getInstance() {
    static Render render_instance;

    return render_instance;
}

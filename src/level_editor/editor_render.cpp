#include "editor_render.h"

void EditorRender::render(sf::RenderWindow& window) {
    render_.render(window);
}

void EditorRender::addEntity(std::weak_ptr<RenderableEntity> entity) {
    render_.addEntity(entity);
}

void EditorRender::moveView(float x, float y) {
    render_.moveView(x, y);
}

void EditorRender::resizeView(float width, float height) {
    render_.resizeView(width, height);
}

void EditorRender::setView(float x, float y, float width, float height) {
    render_.setView(x, y, width, height);
}

sf::View EditorRender::getView() {
    return render_.getView();
}

void EditorRender::renderLayer(sf::RenderWindow& window, RenderableEntity::Layer layer) {
    render_.renderLayer(window, layer);
}

void EditorRender::setParallaxEnabled(bool enable) {
    render_.parallax_enabled_ = enable;
}

bool EditorRender::getParallaxEnabled() {
    return render_.parallax_enabled_;
}

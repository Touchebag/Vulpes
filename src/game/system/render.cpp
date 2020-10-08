#include "system/render.h"

namespace {

std::map<RenderableEntity::Layer, float> parallax_map = {
    {RenderableEntity::Layer::BACKGROUND, 0.0},
    {RenderableEntity::Layer::BG_3, 0.85},
    {RenderableEntity::Layer::BG_2, 0.9},
    {RenderableEntity::Layer::BG_1, 0.95},
    {RenderableEntity::Layer::MAIN_BG, 1.0},
    {RenderableEntity::Layer::MAIN, 1.0},
    {RenderableEntity::Layer::MAIN_FG, 1.0},
    {RenderableEntity::Layer::FG_1, 1.05},
    {RenderableEntity::Layer::FG_2, 1.1},
    {RenderableEntity::Layer::FG_3, 1.15},
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

} // namespace

void Render::renderLayer(sf::RenderWindow& window, RenderableEntity::Layer layer) {
    float parallax_mulitiplier = 1;

    if (parallax_enabled_) {
        parallax_mulitiplier = parallax_map[layer];
    }

    sf::View viewport({view_x_ * parallax_mulitiplier, view_y_ * parallax_mulitiplier}, {view_width_, view_height_});
    window.setView(viewport);

    renderAllEntitesInVector(layers_[static_cast<int>(layer)], window);
}

void Render::moveView(float x, float y) {
    auto width_offset = view_width_ / 2.0f;
    auto height_offset = view_height_ / 2.0f;

    if (x - width_offset < camera_box_.left_margin) {
        x = camera_box_.left_margin + width_offset;
    } else if (x + width_offset > camera_box_.right_margin) {
        x = camera_box_.right_margin - width_offset;
    }

    if (y - height_offset < camera_box_.top_margin) {
        y = camera_box_.top_margin + height_offset;
    } else if (y + height_offset > camera_box_.bottom_margin) {
        y = camera_box_.bottom_margin - height_offset;
    }

    setView(x, y, view_width_, view_height_);
}

void Render::resizeView(float width, float height) {
    setView(view_x_, view_y_, width, height);
}

void Render::setView(float x, float y, float width, float height) {
    view_x_ = x;
    view_y_ = y;
    view_width_ = width;
    view_height_ = height;
}

void Render::setCameraBox(CameraBox camera_box) {
    camera_box_ = camera_box;
}

IRender::CameraBox Render::getCameraBox() {
    return camera_box_;
}

sf::View Render::getView() {
    return {{view_x_, view_y_}, {view_width_, view_height_}};
}

void Render::drawHud(sf::RenderWindow& window) {
    // Set static view for HUD
    window.setView({{500, 500}, {1000, 1000}});
    renderAllEntitesInVector(hud_layer_, window);

    window.setView(getView());
}

void Render::render(sf::RenderWindow& window) {
    for (int i = 0; i < static_cast<int>(RenderableEntity::Layer::MAX_LAYERS); ++i) {
        renderLayer(window, static_cast<RenderableEntity::Layer>(i));
    }

    drawHud(window);
}

void Render::addEntity(std::weak_ptr<RenderableEntity> entity) {
    if (auto ent = entity.lock()) {
        RenderableEntity::Layer layer = ent->getLayer();

        if (layer == RenderableEntity::Layer::HUD) {
            hud_layer_.push_back(entity);
        } else {
            layers_[static_cast<int>(layer)].push_back(entity);
        }
    }
}

#include "system/render.h"

#include "system/system.h"
#include "system/camera.h"

#include "utils/log.h"

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

void renderAllEntitesInVector(std::vector<std::weak_ptr<RenderableEntity>>& layer, sf::RenderWindow& window, float frame_fraction = 0.0f) {
    for (auto it = layer.begin(); it != layer.end(); ) {
        if (auto ptr = it->lock()) {
            ptr->render(window, frame_fraction);
            ++it;
        } else {
            it = layer.erase(it);
        }
    }
}

} // namespace

void Render::renderLayer(sf::RenderWindow& window, float frame_fraction, RenderableEntity::Layer layer) {
    float parallax_mulitiplier = 1;

    if (parallax_enabled_) {
        parallax_mulitiplier = parallax_map[layer];
    }

    auto view = System::getCamera()->getView();
    auto change_speed = System::getCamera()->getChangeVelocity();

    // Note: interpolating size causes warping/bouncing issues
    sf::View viewport(
            {(view.x_pos + (change_speed.x_pos * frame_fraction)) * parallax_mulitiplier,
             (view.y_pos + (change_speed.y_pos * frame_fraction)) * parallax_mulitiplier},
            {view.width ,
             view.height});
    window.setView(viewport);

    renderAllEntitesInVector(layers_[static_cast<int>(layer)], window, frame_fraction);
}

void Render::drawHud(sf::RenderWindow& window) {
    auto current_view = window.getView();

    // Set static view for HUD
    window.setView({{500, 500}, {1000, 1000}});
    renderAllEntitesInVector(hud_layer_, window);

    window.setView(current_view);
}

void Render::render(sf::RenderWindow& window, float frame_fraction) {
    for (int i = 0; i < static_cast<int>(RenderableEntity::Layer::MAX_LAYERS); ++i) {
        renderLayer(window, frame_fraction, static_cast<RenderableEntity::Layer>(i));
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

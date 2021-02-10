#include "system/render.h"

#include "system/system.h"
#include "system/camera.h"

#include "utils/log.h"

namespace {

std::map<int, float> parallax_map = {
    {-5, 0.85},
    {-4, 0.9},
    {-3, 0.95},
    {-2, 1.0},
    {-1, 1.0},
    {0, 1.0},
    {1, 1.0},
    {2, 1.0},
    {3, 1.05},
    {4, 1.1},
    {5, 1.15},
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

Render::Render() :
    background_layers_(5),
    foreground_layers_(5) {
}

void Render::renderLayer(sf::RenderWindow& window, float frame_fraction, int layer) {
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

    renderAllEntitesInVector(getLayer(layer), window, frame_fraction);
}

void Render::drawHud(sf::RenderWindow& window) {
    auto current_view = window.getView();

    // Set static view for HUD
    window.setView({{500, 500}, {1000, 1000}});
    renderAllEntitesInVector(hud_layer_, window);

    window.setView(current_view);
}

void Render::render(sf::RenderWindow& window, float frame_fraction) {
    for (int i = -static_cast<int>(background_layers_.size()); i <= -1; ++i) {
        renderLayer(window, frame_fraction, i);
    }

    renderLayer(window, frame_fraction, 0);

    if (auto player = player_.lock()) {
        player->render(window, frame_fraction);
    }

    for (int i = 1; i <= static_cast<int>(foreground_layers_.size()); ++i) {
        renderLayer(window, frame_fraction, i);
    }

    drawHud(window);
}

void Render::setPlayer(std::weak_ptr<RenderableEntity> entity) {
    player_ = entity;
}

std::vector<std::weak_ptr<RenderableEntity>>& Render::getLayer(int layer) {
    if (layer < 0) {
        try {
            return background_layers_.at(abs(layer) - 1);
        } catch (std::out_of_range& e) {
            LOGE("Render background, layer out of range");
            throw e;
        }
    } else if (layer > 0) {
        try {
            return foreground_layers_.at(layer - 1);
        } catch (std::out_of_range& e) {
            LOGE("Render foreground, layer out of range");
            throw e;
        }
    } else {
        return main_layer_;
    }
}

void Render::addEntity(std::weak_ptr<RenderableEntity> entity) {
    if (auto ent = entity.lock()) {
        int layer = ent->getLayer();

        if (layer == INT_MAX) {
            hud_layer_.push_back(entity);
            return;
        }

        getLayer(layer).push_back(entity);
    }
}

#include "system/render.h"

#include "system/system.h"
#include "system/camera.h"

#include "utils/log.h"

namespace {

const std::map<int, float> parallax_map = {
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

void renderAllEntitesInVector(std::vector<std::weak_ptr<RenderableEntity>>& layer, sf::RenderTarget& target, float frame_fraction = 0.0f) {
    for (auto it = layer.begin(); it != layer.end(); ) {
        if (auto ptr = it->lock()) {
            ptr->render(target, frame_fraction);
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
    for (int i = -5; i <= 5; i++) {
        getLayer(i).parallax_multiplier = parallax_map.at(i);
    }
    std::shared_ptr<sf::Shader> shader;

    getLayer(-5).shaders.clear();
    shader = File().loadShader("color_fade.frag");
    shader->setUniform("target_color", sf::Glsl::Vec3(0.5f, 0.8f, 1.0f));
    shader->setUniform("intensity", 0.3f);
    getLayer(-5).shaders.push_back(shader);

    getLayer(-4).shaders.clear();
    shader = File().loadShader("color_fade.frag");
    shader->setUniform("target_color", sf::Glsl::Vec3(0.5f, 0.8f, 1.0f));
    shader->setUniform("intensity", 0.15f);
    getLayer(-4).shaders.push_back(shader);

    getLayer(-3).shaders.clear();
    shader = File().loadShader("color_fade.frag");
    shader->setUniform("target_color", sf::Glsl::Vec3(0.5f, 0.8f, 1.0f));
    shader->setUniform("intensity", 0.05f);
    getLayer(-3).shaders.push_back(shader);

    getLayer(3).shaders.clear();
    shader = File().loadShader("blur.frag");
    shader->setUniform("direction", sf::Glsl::Vec2(1.0, 0.0));
    getLayer(3).shaders.push_back(shader);
    shader = File().loadShader("blur.frag");
    shader->setUniform("direction", sf::Glsl::Vec2(0.0, 1.0));
    getLayer(3).shaders.push_back(shader);

    getLayer(4).shaders.clear();
    shader = File().loadShader("blur.frag");
    shader->setUniform("direction", sf::Glsl::Vec2(1.0, 0.0));
    getLayer(4).shaders.push_back(shader);
    shader = File().loadShader("blur.frag");
    shader->setUniform("direction", sf::Glsl::Vec2(0.0, 1.0));
    getLayer(4).shaders.push_back(shader);

    getLayer(5).shaders.clear();
    shader = File().loadShader("blur.frag");
    shader->setUniform("direction", sf::Glsl::Vec2(1.0, 0.0));
    getLayer(5).shaders.push_back(shader);
    shader = File().loadShader("blur.frag");
    shader->setUniform("direction", sf::Glsl::Vec2(0.0, 1.0));
    getLayer(5).shaders.push_back(shader);
}

void Render::renderLayerWithPostProcessing(sf::RenderWindow& window, int layer, float frame_fraction) {
    render_texture_.clear(sf::Color(0, 0, 0, 0));

    renderLayer(render_texture_, frame_fraction, layer);
    render_texture_.display();
    bool render_from_primary = true;

    if (!getLayer(layer).shaders.empty()) {
        for (auto shader : getLayer(layer).shaders) {
            if (render_from_primary) {
                secondary_render_texture_.clear(sf::Color(0, 0, 0, 0));
                secondary_render_texture_.setView(window.getView());
                secondary_render_texture_.draw(render_layer_sprite_, shader.get());
                secondary_render_texture_.display();
                render_from_primary = false;
            } else {
                render_texture_.clear(sf::Color(0, 0, 0, 0));
                render_texture_.setView(window.getView());
                render_texture_.draw(secondary_render_layer_sprite_, shader.get());
                render_texture_.display();
                render_from_primary = true;
            }
        }
    }

    if (render_from_primary) {
        window.draw(render_layer_sprite_);
    } else {
        window.draw(secondary_render_layer_sprite_);
    }
}

void Render::renderLayer(sf::RenderTarget& target, float frame_fraction, int layer) {
    float parallax_multiplier = 1;

    if (parallax_enabled_) {
        parallax_multiplier = getLayer(layer).parallax_multiplier;
    }

    auto view = System::getCamera()->getView();
    auto change_speed = System::getCamera()->getChangeVelocity();

    // Note: interpolating size causes warping/bouncing issues
    sf::View viewport(
            {(view.x_pos + (change_speed.x_pos * frame_fraction)) * parallax_multiplier,
             (view.y_pos + (change_speed.y_pos * frame_fraction)) * parallax_multiplier},
            {view.width ,
             view.height});
    target.setView(viewport);

    renderAllEntitesInVector(getLayerRenderables(layer), target, frame_fraction);
}

void Render::drawPlayer(sf::RenderWindow& window, float frame_fraction) {
    if (auto player = player_.lock()) {
        auto camera = System::getCamera();

        render_texture_.clear(sf::Color(0, 0, 0, 0));

        auto view = camera->getView();
        auto change_speed = System::getCamera()->getChangeVelocity();

        // Note: interpolating size causes warping/bouncing issues
        sf::View viewport(
                {(view.x_pos + (change_speed.x_pos * frame_fraction)),
                 (view.y_pos + (change_speed.y_pos * frame_fraction))},
                {view.width ,
                 view.height});
        render_texture_.setView(viewport);

        player->render(render_texture_, frame_fraction);

        render_texture_.display();
        render_layer_sprite_.setTexture(render_texture_.getTexture());
        window.draw(render_layer_sprite_);
    }
}

void Render::drawHud(sf::RenderWindow& window) {
    auto current_view = window.getView();

    // Set static view for HUD
    window.setView({{500, 500}, {1000, 1000}});
    renderAllEntitesInVector(hud_layer_.renderables, window);

    window.setView(current_view);
}

void Render::render(sf::RenderWindow& window, float frame_fraction) {
    // TODO Use actual background layer
    window.clear(sf::Color(135, 206, 255));

    for (int i = -static_cast<int>(background_layers_.size()); i <= -1; ++i) {
        renderLayerWithPostProcessing(window, i, frame_fraction);
    }

    renderLayerWithPostProcessing(window, 0, frame_fraction);

    drawPlayer(window, frame_fraction);

    for (int i = 1; i <= static_cast<int>(foreground_layers_.size()); ++i) {
        renderLayerWithPostProcessing(window, i, frame_fraction);
    }

    drawHud(window);
}

void Render::setPlayer(std::weak_ptr<RenderableEntity> entity) {
    player_ = entity;
}

void Render::setWindowSize(sf::RenderWindow& window, int width, int height) {
    auto view = window.getView();
    view.setSize({static_cast<float>(width), static_cast<float>(height)});
    window.setView(view);
    System::getCamera()->setWindowSize(width, height);

    render_texture_.create(width, height);
    render_texture_.clear(sf::Color(0, 0, 0, 0));
    render_layer_sprite_ = sf::Sprite(render_texture_.getTexture());

    secondary_render_texture_.create(width, height);
    secondary_render_texture_.clear(sf::Color(0, 0, 0, 0));
    secondary_render_layer_sprite_ = sf::Sprite(secondary_render_texture_.getTexture());

    // Set sprite origin to center and reposition to stay in center of window
    auto window_center = window.getView().getCenter();

    render_layer_sprite_.setOrigin(static_cast<float>(width) / 2.0f, static_cast<float>(height) / 2.0f);
    render_layer_sprite_.setPosition(window_center.x, window_center.y);

    secondary_render_layer_sprite_.setOrigin(static_cast<float>(width) / 2.0f, static_cast<float>(height) / 2.0f);
    secondary_render_layer_sprite_.setPosition(window_center.x, window_center.y);
}

Render::RenderLayer& Render::getLayer(int layer) {
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

std::vector<std::weak_ptr<RenderableEntity>>& Render::getLayerRenderables(int layer) {
    return getLayer(layer).renderables;
}

void Render::addEntity(std::weak_ptr<RenderableEntity> entity) {
    if (auto ent = entity.lock()) {
        int layer = ent->getLayer();

        if (layer == INT_MAX) {
            hud_layer_.renderables.push_back(entity);
            return;
        }

        getLayerRenderables(layer).push_back(entity);
    }
}

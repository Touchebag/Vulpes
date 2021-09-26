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

void renderAllEntitesInVector(std::vector<std::weak_ptr<Rendering>>& layer,
                              sf::RenderTarget& target,
                              float frame_fraction = 0.0f) {
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

    to_render_texture_ = std::make_shared<sf::RenderTexture>();
    from_render_texture_ = std::make_shared<sf::RenderTexture>();
}

void Render::renderLayerWithPostProcessing(sf::RenderWindow& window, int layer, float frame_fraction) {
    // Render layer without shaders
    to_render_texture_->clear(sf::Color(0, 0, 0, 0));
    renderLayer(*to_render_texture_, frame_fraction, layer);
    to_render_texture_->display();

    if (!getLayer(layer).shaders.empty()) {
        for (auto shader_handle : getLayer(layer).shaders) {
            if (shader_handle) {
                // Set previous render texture as base for next render
                std::swap(to_render_texture_, from_render_texture_);

                to_render_texture_->clear(sf::Color(0, 0, 0, 0));
                to_render_texture_->setView(window.getView());
                shader_handle->update();
                to_render_texture_->draw(sf::Sprite(from_render_texture_->getTexture()), shader_handle->getShader());
                to_render_texture_->display();
            }
        }
    }

    window.draw(sf::Sprite(to_render_texture_->getTexture()));
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

        to_render_texture_->clear(sf::Color(0, 0, 0, 0));

        auto view = camera->getView();
        auto change_speed = System::getCamera()->getChangeVelocity();

        // Note: interpolating size causes warping/bouncing issues
        sf::View viewport(
                {(view.x_pos + (change_speed.x_pos * frame_fraction)),
                 (view.y_pos + (change_speed.y_pos * frame_fraction))},
                {view.width ,
                 view.height});
        to_render_texture_->setView(viewport);

        player->render(*to_render_texture_, frame_fraction);

        to_render_texture_->display();
        window.draw(sf::Sprite(to_render_texture_->getTexture()));
    }
}

void Render::drawBackground(sf::RenderWindow& window) {
    auto current_view = window.getView();

    auto size = background_.getSize();
    // Set static view for background
    window.setView({{static_cast<float>(size.x) / 2.0f, static_cast<float>(size.y) / 2.0f},
                    {static_cast<float>(size.x), static_cast<float>(size.y)}});
    window.draw(sf::Sprite(background_));

    window.setView(current_view);
}

void Render::drawHud(sf::RenderWindow& window) {
    auto current_view = window.getView();

    // Set static view for HUD
    window.setView({{500, 500}, {1000, 1000}});
    renderAllEntitesInVector(hud_layer_.renderables, window);

    window.setView(current_view);
}

void Render::render(sf::RenderWindow& window, float frame_fraction) {
    window.clear(sf::Color(0, 0, 0));

    drawBackground(window);

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

void Render::setPlayer(std::weak_ptr<Rendering> entity) {
    player_ = entity;
}

void Render::setBackground(std::string background) {
    if (auto texture = File().loadTexture(background)) {
        background_ = texture.value();
    } else {
        LOGW("Failed to set background %s", background.c_str());
    }
}

void Render::setWindowSize(sf::RenderWindow& window, int width, int height) {
    auto view = window.getView();
    view.setSize({static_cast<float>(width), static_cast<float>(height)});
    view.setCenter({static_cast<float>(width) / 2.0f, static_cast<float>(height) / 2.0f});

    window.setView(view);
    System::getCamera()->setWindowSize(width, height);

    to_render_texture_->create(width, height);
    to_render_texture_->clear(sf::Color(0, 0, 0, 0));

    from_render_texture_->create(width, height);
    from_render_texture_->clear(sf::Color(0, 0, 0, 0));
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

std::vector<std::weak_ptr<Rendering>>& Render::getLayerRenderables(int layer) {
    return getLayer(layer).renderables;
}

void Render::addEntity(std::weak_ptr<Rendering> entity) {
    if (auto ent = entity.lock()) {
        int layer = ent->getLayer();

        if (layer == INT_MAX) {
            hud_layer_.renderables.push_back(entity);
            return;
        }

        getLayerRenderables(layer).push_back(entity);
    }
}

void Render::addShader(std::shared_ptr<ShaderHandle> shader, int layer) {
    getLayer(layer).shaders.push_back(shader);
}

void Render::clearLayerShaders() {
    for (auto& layer : background_layers_) {
        layer.shaders.clear();
    }
    for (auto& layer : foreground_layers_) {
        layer.shaders.clear();
    }
}

void Render::loadLayerShaders(nlohmann::json j) {
    for (auto& it : j) {
        if (!it.contains("layer")) {
            throw std::invalid_argument("Shader missing layer");
        }

        auto layer = it["layer"].get<int>();
        for (auto shader : it["shaders"]) {
            addShader(ShaderHandle::createFromJson(shader), layer);
        }
    }
}

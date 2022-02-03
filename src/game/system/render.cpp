#include "system/render.h"

#include "system/system.h"
#include "system/camera.h"

#include "utils/log.h"

namespace {

const std::map<int, double> parallax_map = {
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
                              double frame_fraction = 0.0) {
    for (auto it = layer.begin(); it != layer.end(); ) {
        if (auto ptr = it->lock()) {
            ptr->render(target, frame_fraction);
            ++it;
        } else {
            it = layer.erase(it);
        }
    }
}

// Renders anything from front_buffer plus any shaders into target
void renderShadersWithDoubleBuffer(sf::RenderTarget& target,
                                   std::shared_ptr<sf::RenderTexture> front_buffer,
                                   std::shared_ptr<sf::RenderTexture> back_buffer,
                                   std::vector<std::weak_ptr<ShaderHandle>>& shaders) {
    for (auto it = shaders.begin(); it < shaders.end(); it++) {
        if (auto shader_handle = it->lock()) {
            // Always render into back buffer
            back_buffer->clear(sf::Color(0, 0, 0, 0));
            back_buffer->setView(target.getView());
            back_buffer->draw(sf::Sprite(front_buffer->getTexture()), shader_handle->getShader());

            // Swap buffers and display new front buffer
            std::swap(front_buffer, back_buffer);
            front_buffer->display();
        } else {
            it = shaders.erase(it);
        }
    }

    target.draw(sf::Sprite(front_buffer->getTexture()));
}

} // namespace

Render::Render() :
        background_layers_(5),
        foreground_layers_(5) {
    for (int i = -5; i <= 5; i++) {
        getLayer(i).parallax_multiplier = parallax_map.at(i);
    }

    front_render_buffer_ = std::make_shared<sf::RenderTexture>();
    back_render_buffer_  = std::make_shared<sf::RenderTexture>();
    render_texture_      = std::make_shared<sf::RenderTexture>();
}

void Render::renderLayerWithPostProcessing(sf::RenderTarget& window, int layer, double frame_fraction) {
    // First render layer without shaders
    front_render_buffer_->clear(sf::Color(0, 0, 0, 0));
    renderLayer(*front_render_buffer_, frame_fraction, layer);
    front_render_buffer_->display();

    auto& layer_shaders = getLayer(layer).shaders;
    renderShadersWithDoubleBuffer(window, front_render_buffer_, back_render_buffer_, layer_shaders);
}

void Render::renderLayer(sf::RenderTarget& target, double frame_fraction, int layer) {
    double parallax_multiplier = 1;

    if (parallax_enabled_) {
        parallax_multiplier = getLayer(layer).parallax_multiplier;
    }

    auto view = System::getCamera()->getView();
    auto change_speed = System::getCamera()->getChangeVelocity();

    // Note: interpolating size causes warping/bouncing issues
    sf::View viewport(
            {static_cast<float>((view.x_pos + (change_speed.x_pos * frame_fraction)) * parallax_multiplier),
             static_cast<float>((view.y_pos + (change_speed.y_pos * frame_fraction)) * parallax_multiplier)},
            {static_cast<float>(view.width),
             static_cast<float>(view.height)});
    target.setView(viewport);

    renderAllEntitesInVector(getLayerRenderables(layer), target, frame_fraction);
}

void Render::drawPlayer(sf::RenderTarget& window, double frame_fraction) {
    if (auto player = player_.lock()) {
        auto camera = System::getCamera();

        front_render_buffer_->clear(sf::Color(0, 0, 0, 0));

        auto view = camera->getView();
        auto change_speed = System::getCamera()->getChangeVelocity();

        // Note: interpolating size causes warping/bouncing issues
        sf::View viewport(
                {static_cast<float>(view.x_pos + (change_speed.x_pos * frame_fraction)),
                 static_cast<float>(view.y_pos + (change_speed.y_pos * frame_fraction))},
                {static_cast<float>(view.width),
                 static_cast<float>(view.height)});
        front_render_buffer_->setView(viewport);

        player->render(*front_render_buffer_, frame_fraction);

        front_render_buffer_->display();
        window.draw(sf::Sprite(front_render_buffer_->getTexture()));
    }
}

void Render::drawBackground(sf::RenderTarget& window) {
    auto current_view = window.getView();

    auto size = background_.getSize();
    // Set static view for background
    window.setView({{static_cast<float>(size.x) / 2.0f, static_cast<float>(size.y) / 2.0f},
                    {static_cast<float>(size.x), static_cast<float>(size.y)}});
    window.draw(sf::Sprite(background_));

    window.setView(current_view);
}

void Render::drawHud(sf::RenderTarget& window) {
    auto current_view = window.getView();

    // Set static view for HUD
    window.setView({{500, 500}, {1000, 1000}});
    renderAllEntitesInVector(hud_layer_.renderables, window);

    window.setView(current_view);
}

void Render::render(sf::RenderTarget& window, double frame_fraction) {
    window.clear(sf::Color(0, 0, 0));
    render_texture_->clear(sf::Color(0, 0, 0));

    drawBackground(*render_texture_);

    for (int i = -static_cast<int>(background_layers_.size()); i <= -1; ++i) {
        renderLayerWithPostProcessing(*render_texture_, i, frame_fraction);
    }

    renderLayerWithPostProcessing(*render_texture_, 0, frame_fraction);

    drawPlayer(*render_texture_, frame_fraction);

    for (int i = 1; i <= static_cast<int>(foreground_layers_.size()); ++i) {
        renderLayerWithPostProcessing(*render_texture_, i, frame_fraction);
    }

    render_texture_->display();

    renderShadersWithDoubleBuffer(window,
                                  render_texture_,
                                  back_render_buffer_,
                                  global_shaders_);

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

void Render::setWindowSize(sf::RenderWindow& window, unsigned int width, unsigned int height) {
    auto view = window.getView();
    view.setSize({static_cast<float>(width), static_cast<float>(height)});
    view.setCenter({static_cast<float>(width) / 2.0f, static_cast<float>(height) / 2.0f});

    window.setView(view);
    System::getCamera()->setWindowSize(width, height);

    front_render_buffer_->create(width, height);
    front_render_buffer_->clear(sf::Color(0, 0, 0, 0));

    back_render_buffer_->create(width, height);
    back_render_buffer_->clear(sf::Color(0, 0, 0, 0));

    render_texture_->create(width, height);
    render_texture_->clear(sf::Color(0, 0, 0, 0));

}

Render::RenderLayer& Render::getLayer(int layer) {
    if (layer < 0) {
        try {
            return background_layers_.at(static_cast<unsigned int>(abs(layer) - 1));
        } catch (std::out_of_range& e) {
            LOGE("Render background, layer out of range");
            throw e;
        }
    } else if (layer > 0) {
        try {
            return foreground_layers_.at(static_cast<unsigned int>(layer - 1));
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

void Render::addGlobalShader(std::shared_ptr<ShaderHandle> shader) {
    global_shaders_.push_back(shader);
}

void Render::clearShaders() {
    for (auto& layer : background_layers_) {
        layer.shaders.clear();
    }
    for (auto& layer : foreground_layers_) {
        layer.shaders.clear();
    }
}

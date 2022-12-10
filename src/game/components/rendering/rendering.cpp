#include "components/rendering/rendering.h"

#include "utils/log.h"
#include "utils/file.h"
#include "utils/common.h"
#include "components/component_store.h"

Rendering::Rendering(std::weak_ptr<ComponentStore> components) :
        Component(components) {
    front_render_texture_ = std::make_shared<sf::RenderTexture>();
    back_render_texture_ = std::make_shared<sf::RenderTexture>();
}

void Rendering::setTexture(std::shared_ptr<sf::Texture> texture) {
    texture_ = texture;

    sprite_.setTexture(*texture_, true);
    original_texture_rect_ = sprite_.getTextureRect();
    texture_->setRepeated(true);

    recalculateTextureRect();

}

bool Rendering::loadTexture(std::string file_path, File file_instance) {
    if (std::optional<sf::Texture> texture = file_instance.loadTexture(file_path)) {
        setTexture(std::make_shared<sf::Texture>(texture.value()));
        texture_name_ = file_path;
    } else {
        LOGW("Error loading image %s", file_path.c_str());
        return false;
    }
    return true;
}

std::shared_ptr<Rendering> Rendering::createFromJson(nlohmann::json j, std::weak_ptr<ComponentStore> components, File file_instance) {
    auto ret_ptr = std::make_shared<Rendering>(components);

    ret_ptr->reloadFromJson(j, file_instance);

    return ret_ptr;
}

void Rendering::reloadFromJson(nlohmann::json j, File file_instance) {
    if (j.contains("height")) {
        height_ = j["height"];
    }

    if (j.contains("width")) {
        width_ = j["width"];
    }

    if (j.contains("tile_x")) {
        tiling_x_ = j["tile_x"].get<int>();
    } else {
        tiling_x_ = 1;
    }

    if (j.contains("tile_y")) {
        tiling_y_ = j["tile_y"].get<int>();
    } else {
        tiling_y_ = 1;
    }

    if (j.contains("layer")) {
        layer_ = j["layer"].get<int>();
    } else {
        throw std::invalid_argument("Entity missing layer");
    }

    if (j.contains("texture")) {
        loadTexture(j["texture"].get<std::string>(), file_instance);
    }

    if (j.contains("shaders")) {
        shaders_.clear();
        for (auto shader : j["shaders"]) {
            loadShader(shader);
        }

        if (texture_) {
            auto size = texture_->getSize();

            front_render_texture_->create(size.x, size.y);
            front_render_texture_->clear(sf::Color(0, 0, 0, 0));

            back_render_texture_->create(size.x, size.y);
            back_render_texture_->clear(sf::Color(0, 0, 0, 0));
        }
    }

    if (!texture_ && shaders_.empty()) {
        LOGW("Rendering: No texture or shader");
    }
}

void Rendering::recalculateTextureRect() {
    int texture_rect_x;
    int texture_rect_y;

    if (width_ == 0) {
        texture_rect_x = original_texture_rect_.width;
    } else if (tiling_x_ == 0) {
        texture_rect_x = width_;
    } else {
        texture_rect_x = original_texture_rect_.width * tiling_x_;
    }

    if (height_ == 0) {
        texture_rect_y = original_texture_rect_.height;
    } else if (tiling_y_ == 0) {
        texture_rect_y = height_;
    } else {
        texture_rect_y = original_texture_rect_.height * tiling_y_;
    }

    setTextureCoords(0, 0, texture_rect_x, texture_rect_y);
}

std::pair<int, int> Rendering::getTiling() {
    return {tiling_x_, tiling_y_};
}

void Rendering::setTiling(int tiling_x, int tiling_y) {
    tiling_x_ = tiling_x;
    tiling_y_ = tiling_y;

    recalculateTextureRect();
}

std::optional<nlohmann::json> Rendering::outputToJson() {
    nlohmann::json j;

    j["texture"] = texture_name_;

    if (height_ > 0) {
        j["height"] = height_;
    }

    if (width_ > 0) {
        j["width"] = width_;
    }

    if (tiling_x_ != 1) {
        j["tile_x"] = tiling_x_;
    }

    if (tiling_y_ != 1) {
        j["tile_y"] = tiling_y_;
    }

    j["layer"] = layer_;

    return {j};
}

void Rendering::setTextureCoords(int pos_x, int pos_y, int width, int height) {
    sprite_.setOrigin(static_cast<float>(width / 2.0), static_cast<float>(height / 2.0));
    sprite_.setTextureRect(sf::IntRect(pos_x, pos_y, width, height));

    // Mirror sprites facing left
    auto mirror_scale = 1.0;
    if (auto move = getComponent<Movement>()) {
        mirror_scale = move->isFacingRight() ? 1.0 : -1.0;
    }

    // Scale
    double x_scale = 1.0;
    double y_scale = 1.0;

    if (width_ > 0 && height_ > 0) {
        x_scale = static_cast<double>(width_) / static_cast<double>(width);
        y_scale = static_cast<double>(height_) / static_cast<double>(height);
    } else if (width_ > 0) {
        x_scale = static_cast<double>(width_) / static_cast<double>(width);
        y_scale = x_scale;
    } else if (height_ > 0) {
        y_scale = static_cast<double>(height_) / static_cast<double>(height);
        x_scale = y_scale;
    }

    sprite_.setScale(static_cast<float>(mirror_scale * x_scale * x_scale_),
                     static_cast<float>(y_scale * y_scale_));
}

void Rendering::setSize(int width, int height) {
    width_ = width;
    height_ = height;

    recalculateTextureRect();
}

std::pair<int, int> Rendering::getSize() {
    return {width_, height_};
}

std::pair<double, double> Rendering::getScaledSize() {
    auto size_rect = sprite_.getGlobalBounds();
    return {size_rect.width, size_rect.height};
}

sf::Color Rendering::getColor() {
    return sprite_.getColor();
}

void Rendering::setColor(sf::Color color) {
    sprite_.setColor(color);
}

void Rendering::clearColor() {
    sprite_.setColor({255, 255, 255, 255});
}

void Rendering::render(sf::RenderTarget& target, double frame_fraction) {
    // Always update shader uniforms
    for (auto shader_handle : shaders_) {
        if (shader_handle) {
            shader_handle->update();
        }
    }

    // Do not render if texture missing
    // Used for global shaders
    if (!texture_) {
        return;
    }

    // Used for interpolation between physics frames
    auto vel_x = 0.0;
    auto vel_y = 0.0;

    auto x_offset = x_offset_;
    auto y_offset = y_offset_;

    if (auto move = getComponent<Movement>()) {
        vel_x = move->getVelX() * frame_fraction;
        vel_y = move->getVelY() * frame_fraction;

        if (!move->isFacingRight()) {
            x_offset = -x_offset;
        }
    }

    if (auto trans = getComponent<Transform>()) {
        sprite_.setPosition(
                static_cast<float>(trans->getX() + x_offset + vel_x),
                static_cast<float>(trans->getY() + y_offset + vel_y));

        if (!shaders_.empty()) {
            // Render sprite without shaders first
            sf::Sprite shader_sprite(*texture_);

            front_render_texture_->clear(sf::Color(0, 0, 0, 0));
            front_render_texture_->draw(shader_sprite);
            front_render_texture_->display();

            for (auto shader_handle : shaders_) {
                if (shader_handle) {
                    // Set previous render texture as base for next render
                    std::swap(front_render_texture_, back_render_texture_);

                    front_render_texture_->clear(sf::Color(0, 0, 0, 0));
                    front_render_texture_->draw(sf::Sprite(back_render_texture_->getTexture()), shader_handle->getShader());
                    front_render_texture_->display();
                }
            }

            sprite_.setTexture(front_render_texture_->getTexture());
        }

        target.draw(sprite_);
    } else {
        LOGW("Rendering: Missing transform");
    }
}

int Rendering::getLayer() {
    return layer_;
}

void Rendering::setLayer(int layer) {
    layer_ = layer;
}

void Rendering::loadShader(nlohmann::json j) {
    shaders_.push_back(ShaderHandle::createFromJson(j, component_store_));
}

std::vector<std::shared_ptr<ShaderHandle>> Rendering::getShaders() {
    return shaders_;
}

void Rendering::setScale(double x_scale, double y_scale) {
    x_scale_ = x_scale;
    y_scale_ = y_scale;
}

void Rendering::setOffset(int x_offset, int y_offset) {
    x_offset_ = x_offset;
    y_offset_ = y_offset;
}

void Rendering::update() {
}

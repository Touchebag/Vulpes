#include "components/rendering/rendering.h"
#include "utils/log.h"
#include "utils/file.h"
#include "utils/common.h"

RenderableEntity::RenderableEntity(std::weak_ptr<ComponentStore> components) :
    Component(components) {
}

bool RenderableEntity::loadTexture(std::string file_path, File file_instance) {
    if (std::optional<sf::Texture> texture = file_instance.loadTexture(file_path)) {
        texture_ = texture.value();
    } else {
        LOGW("Error loading image %s", file_path.c_str());
        return false;
    }

    sprite_.setTexture(texture_, true);
    original_texture_rect_ = sprite_.getTextureRect();
    texture_.setRepeated(true);
    texture_name_ = file_path;

    recalculateTextureRect();

    return true;
}

std::shared_ptr<RenderableEntity> RenderableEntity::createFromJson(nlohmann::json j, std::weak_ptr<ComponentStore> components, File file_instance) {
    auto ret_ptr = std::make_shared<RenderableEntity>(components);

    ret_ptr->reloadFromJson(j, file_instance);

    return ret_ptr;
}

void RenderableEntity::reloadFromJson(nlohmann::json j, File file_instance) {
    if (j.contains("height")) {
        height_ = j["height"];
    }

    if (j.contains("width")) {
        width_ = j["width"];
    }

    if (j.contains("tile_x")) {
        tiling_x_ = j["tile_x"].get<int>();
    }

    if (j.contains("tile_y")) {
        tiling_y_ = j["tile_y"].get<int>();
    }

    if (j.contains("layer")) {
        layer_ = j["layer"].get<int>();
    } else {
        throw std::invalid_argument("Entity missing layer");
    }

    loadTexture(j["texture"].get<std::string>(), file_instance);
}

void RenderableEntity::recalculateTextureRect() {
    int texture_rect_x;
    int texture_rect_y;

    if (tiling_x_ == 0) {
        texture_rect_x = width_;
    } else {
        texture_rect_x = original_texture_rect_.width * tiling_x_;
    }

    if (tiling_y_ == 0) {
        texture_rect_y = height_;
    } else {
        texture_rect_y = original_texture_rect_.height * tiling_y_;
    }

    setTextureCoords(0, 0, texture_rect_x, texture_rect_y);
}

std::pair<int, int> RenderableEntity::getTiling() {
    return {tiling_x_, tiling_y_};
}

void RenderableEntity::setTiling(int tiling_x, int tiling_y) {
    tiling_x_ = tiling_x;
    tiling_y_ = tiling_y;

    recalculateTextureRect();
}

std::optional<nlohmann::json> RenderableEntity::outputToJson() {
    nlohmann::json j;

    j["texture"] = texture_name_;

    if (height_ > 0) {
        j["height"] = height_;
    }

    if (width_ > 0) {
        j["width"] = width_;
    }

    j["tile_x"] = tiling_x_;
    j["tile_y"] = tiling_y_;

    j["layer"] = layer_;

    return {j};
}

void RenderableEntity::setTextureCoords(int pos_x, int pos_y, int width, int height) {
    sprite_.setOrigin(static_cast<float>(width / 2.0), static_cast<float>(height / 2.0));
    sprite_.setTextureRect(sf::IntRect(pos_x, pos_y, width, height));

    // Mirror sprites facing left
    auto mirror_scale = 1.0;
    if (auto move = movable_.lock()) {
        mirror_scale = move->facing_right_ ? 1.0 : -1.0;
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

void RenderableEntity::setSize(int width, int height) {
    width_ = width;
    height_ = height;

    recalculateTextureRect();
}

std::pair<int, int> RenderableEntity::getSize() {
    return {width_, height_};
}

std::pair<float, float> RenderableEntity::getScaledSize() {
    auto size_rect = sprite_.getGlobalBounds();
    return {size_rect.width, size_rect.height};
}

void RenderableEntity::setColor(sf::Color color) {
    sprite_.setColor(color);
}

void RenderableEntity::clearColor() {
    sprite_.setColor({255, 255, 255, 255});
}

void RenderableEntity::render(sf::RenderTarget& target, float frame_fraction) {
    // Used for interpolation between physics frames
    auto vel_x = 0.0;
    auto vel_y = 0.0;

    if (auto move = movable_.lock()) {
        vel_x = move->getVelX() * frame_fraction;
        vel_y = move->getVelY() * frame_fraction;
    }

    if (auto trans = trans_.lock()) {
        sprite_.setPosition(static_cast<float>(trans->getX() + vel_x), static_cast<float>(trans->getY() + vel_y));
        if (shader_) {
            target.draw(sprite_, shader_.get());
        } else {
            target.draw(sprite_);
        }
    } else {
        LOGW("Rendering: Missing transform");
    }
}

int RenderableEntity::getLayer() {
    return layer_;
}

void RenderableEntity::setLayer(int layer) {
    layer_ = layer;
}

void RenderableEntity::loadShader(std::string shader_name) {
    auto shader = File().loadShader(shader_name);

    shader_ = shader;
}

void RenderableEntity::setScale(float x_scale, float y_scale) {
    x_scale_ = x_scale;
    y_scale_ = y_scale;
}

void RenderableEntity::update() {
    LOGW("RenderableEntity::update, this should not be called");
}

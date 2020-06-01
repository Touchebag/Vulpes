#include "components/rendering/rendering.h"
#include "utils/log.h"
#include "utils/file.h"

RenderableEntity::RenderableEntity(std::weak_ptr<Transform> trans, std::weak_ptr<MovableEntity> movable) :
    trans_(trans),
    movable_(movable) {
}

bool RenderableEntity::loadTexture(std::string file_path) {
    if (std::optional<sf::Texture> texture = File::loadTexture(file_path)) {
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

void RenderableEntity::reloadFromJson(nlohmann::json j) {
    if (j.contains("height")) {
        height_ = j["height"];
    }

    if (j.contains("width")) {
        width_ = j["width"];
    }

    if (j.contains("tile_x")) {
        tiling_x_ = j["tile_x"].get<bool>();
    }

    if (j.contains("tile_y")) {
        tiling_y_ = j["tile_y"].get<bool>();
    }

    loadTexture(j["texture"].get<std::string>());
}

void RenderableEntity::recalculateTextureRect() {
    int texture_rect_x;
    int texture_rect_y;

    texture_rect_x = tiling_x_ ? width_ : original_texture_rect_.width;
    texture_rect_y = tiling_y_ ? height_ : original_texture_rect_.height;

    setTextureCoords(0, 0, texture_rect_x, texture_rect_y);
}

void RenderableEntity::setTiling(bool tiling_x, bool tiling_y) {
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
        y_scale = y_scale;
    } else if (height_ > 0) {
        y_scale = static_cast<double>(height_) / static_cast<double>(height);
        x_scale = y_scale;
    }

    sprite_.setScale(static_cast<float>(mirror_scale * x_scale), static_cast<float>(y_scale));
}

void RenderableEntity::setSize(int width, int height) {
    width_ = width;
    height_ = height;

    recalculateTextureRect();
}

void RenderableEntity::render(sf::RenderWindow& window) {
    if (auto trans = trans_.lock()) {
        sprite_.setPosition(static_cast<float>(trans->getX()), static_cast<float>(trans->getY()));
        window.draw(sprite_);

    // TODO Move to debug menu
    // sf::RectangleShape rectangle(sf::Vector2f(hitbox_->right_ - hitbox_->left_, hitbox_->bottom_ - hitbox_->top_));
    // rectangle.setPosition(trans_->getX() + hitbox_->left_, trans_->getY() + hitbox_->top_);
    // rectangle.setFillColor(sf::Color(255, 0, 0, 64));
    // window.draw(rectangle);
    }
}

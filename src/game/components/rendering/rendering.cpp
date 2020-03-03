#include "components/rendering/rendering.h"
#include "utils/log.h"
#include "utils/file.h"

RenderableEntity::RenderableEntity(std::weak_ptr<Transform> trans) :
    trans_(trans) {
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

    return true;
}

void RenderableEntity::reloadFromJson(nlohmann::json j) {
    loadTexture(j["texture"].get<std::string>());

    if (j.contains("scale")) {
        scale_ = j["scale"];
    }

    if (j.contains("tile_x")) {
        tiling_x_ = j["tile_x"].get<bool>();
    }

    if (j.contains("tile_y")) {
        tiling_y_ = j["tile_y"].get<bool>();
    }
}

void RenderableEntity::recalculateTextureRect(int width, int height) {
    int texture_rect_x;
    int texture_rect_y;

    texture_rect_x = tiling_x_ ? width : original_texture_rect_.width;
    texture_rect_y = tiling_y_ ? height : original_texture_rect_.height;

    setTextureCoords(0, 0, texture_rect_x, texture_rect_y);
}

void RenderableEntity::setTiling(bool tiling_x, bool tiling_y, int width, int height) {
    tiling_x_ = tiling_x;
    tiling_y_ = tiling_y;

    recalculateTextureRect(width, height);
}

std::optional<nlohmann::json> RenderableEntity::outputToJson() {
    nlohmann::json j;

    j["texture"] = texture_name_;
    // TODO Rounding to some number of digits (2?)
    j["scale"] = scale_;

    j["tile_x"] = tiling_x_;
    j["tile_y"] = tiling_y_;

    return {j};
}

void RenderableEntity::setTextureCoords(int pos_x, int pos_y, int width, int height) {
    sprite_.setOrigin(static_cast<float>(width / 2.0), static_cast<float>(height / 2.0));
    sprite_.setTextureRect(sf::IntRect(pos_x, pos_y, width, height));

    // Mirror sprites facing left
    auto mirror_scale = facing_right_ ? 1.0 : -1.0;

    // Scale
    sprite_.setScale(static_cast<float>(mirror_scale * scale_), static_cast<float>(scale_));
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

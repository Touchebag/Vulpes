#include "components/rendering/rendering.h"
#include "utils/log.h"
#include "utils/file.h"
#include "utils/common.h"

namespace {

std::map<std::string, RenderableEntity::Layer> layerStringMap = {
    {"background", RenderableEntity::Layer::BACKGROUND},
    {"bg3", RenderableEntity::Layer::BG_3},
    {"bg2", RenderableEntity::Layer::BG_2},
    {"bg1", RenderableEntity::Layer::BG_1},
    {"main_bg", RenderableEntity::Layer::MAIN_BG},
    {"main", RenderableEntity::Layer::MAIN},
    {"main_fg", RenderableEntity::Layer::MAIN_FG},
    {"fg1", RenderableEntity::Layer::FG_1},
    {"fg2", RenderableEntity::Layer::FG_2},
    {"fg3", RenderableEntity::Layer::FG_3},
    {"hud", RenderableEntity::Layer::HUD},
};

} // namespace

RenderableEntity::RenderableEntity(std::weak_ptr<Transform> trans, std::weak_ptr<MovableEntity> movable) :
    trans_(trans),
    movable_(movable) {
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

void RenderableEntity::reloadFromJson(nlohmann::json j) {
    reloadFromJson(j, File());
}

void RenderableEntity::reloadFromJson(nlohmann::json j, File file_instance) {
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

    if (j.contains("layer")) {
        auto layer_string = j["layer"].get<std::string>();
        try {
            layer_ = layerStringMap.at(layer_string);
        } catch (std::out_of_range& e) {
            throw std::invalid_argument(std::string("Unknown layer: ") + layer_string.c_str());
        }
    } else {
        throw std::invalid_argument("Entity missing layer");
    }

    loadTexture(j["texture"].get<std::string>(), file_instance);
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

    auto layer = getLayerString(layer_);
    if (layer) {
        j["layer"] = layer.value();
    }

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

    sprite_.setScale(static_cast<float>(mirror_scale * x_scale), static_cast<float>(y_scale));
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

void RenderableEntity::render(sf::RenderWindow& window) {
    if (auto trans = trans_.lock()) {
        sprite_.setPosition(static_cast<float>(trans->getX()), static_cast<float>(trans->getY()));
        if (shader_) {
            window.draw(sprite_, shader_.get());
        } else {
            window.draw(sprite_);
        }
    } else {
        LOGW("Rendering: Missing transform");
    }
}

RenderableEntity::Layer RenderableEntity::getLayer() {
    return layer_;
}

void RenderableEntity::setLayer(Layer layer) {
    layer_ = layer;
}

std::optional<std::string> RenderableEntity::getLayerString(RenderableEntity::Layer layer) {
    for (auto it : layerStringMap) {
        if (it.second == layer) {
            return {it.first};
        }
    }

    return std::nullopt;
}

void RenderableEntity::loadShader(std::string shader_name) {
    auto shader = File().loadShader(shader_name);

    shader_ = shader;
}

void RenderableEntity::update() {
    LOGW("RenderableEntity::update, this should not be called");
}

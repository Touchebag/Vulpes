#include "animated.h"
#include "log.h"

#include <fstream>

AnimatedEntity::AnimatedEntity(std::weak_ptr<RenderableEntity> renderableEntity) :
    renderableEntity_(renderableEntity) {
}

void AnimatedEntity::loadSpriteMap(const std::string& path) {
    std::ifstream fs("assets/" + path);

    if (!fs) {
        LOGE("Failed to open file %s", path.c_str());
        exit(1);
    }

    std::vector<std::string> map_str(std::istream_iterator<std::string>{fs},
                                     std::istream_iterator<std::string>());

    for (auto it = map_str.begin(); it != map_str.end(); ++it) {
        // File name
        std::string name = *it;
        ++it;
        // Skip equals sign
        ++it;
        // Positions
        int x_pos = std::stoi(*it);
        ++it;
        int y_pos = std::stoi(*it);
        ++it;
        // Size
        int width = std::stoi(*it);
        ++it;
        int height = std::stoi(*it);

        sprite_sheet_map_.insert({name, {x_pos, y_pos, width, height}});
    }

    sprite_map_ = path;
}

void AnimatedEntity::setFrameList(std::vector<std::string> frame_list) {
    frame_list_ = frame_list;
}

util::Rectangle AnimatedEntity::getSpriteRect() {
    auto sprite = frame_list_.at(current_frame_);
    return sprite_sheet_map_.at(sprite);
}

void AnimatedEntity::loadFromJson(nlohmann::json j) {
    sprite_map_ = j["sprite_map"].get<std::string>();

    loadSpriteMap(sprite_map_);

    nlohmann::json frame_names_array = j["frame_list"];

    for (auto it : frame_names_array) {
        frame_list_.push_back(it.get<std::string>());
    }

    setRenderTexture();
}

std::optional<nlohmann::json> AnimatedEntity::outputToJson() {
    nlohmann::json j;

    j["sprite_map"] = sprite_map_;
    j["frame_list"] = frame_list_;

    return j;
}

void AnimatedEntity::update() {
    if (++current_frame_ >= static_cast<int>(frame_list_.size())) {
        current_frame_ = 0;
    }

    setRenderTexture();
}

void AnimatedEntity::setRenderTexture() {
    if (auto renderable = renderableEntity_.lock()) {
        auto sprite_rect = getSpriteRect();
        renderable->setTextureCoords(sprite_rect.x, sprite_rect.y, sprite_rect.width, sprite_rect.height);
    }
}


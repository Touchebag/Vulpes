#include <fstream>

#include "animation.h"
#include "utils/log.h"
#include "utils/file.h"

AnimatedEntity::AnimatedEntity(std::weak_ptr<RenderableEntity> renderableEntity) :
    renderableEntity_(renderableEntity) {
}

void AnimatedEntity::loadSpriteMap(const std::string& path) {
    auto fs = File::openSpriteMapFile(path);

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
}

void AnimatedEntity::setFrameList(std::vector<std::string> frame_list) {
    frame_list_ = frame_list;
}

util::Rectangle AnimatedEntity::getSpriteRect() {
    try {
        auto sprite = frame_list_.at(current_frame_);
        return sprite_sheet_map_.at(sprite);
    } catch (std::out_of_range& e) {
        LOGE("Could not find sprite map entry");
        throw e;
    }
}

void AnimatedEntity::reloadFromJson(nlohmann::json j) {
    std::string main_entity_name = "common";
    if (j.contains("main_entity_name")) {
        main_entity_name = j["main_entity_name"].get<std::string>();
    }

    loadSpriteMap(main_entity_name);

    nlohmann::json frame_names_array = j["frame_list"];

    frame_list_.clear();
    for (auto it : frame_names_array) {
        frame_list_.push_back(it.get<std::string>());
    }

    original_frame_list_ = frame_list_;

    setRenderTexture();
}

std::optional<nlohmann::json> AnimatedEntity::outputToJson() {
    nlohmann::json j;

    j["frame_list"] = original_frame_list_;

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
    } else {
        LOGW("Animated: Missing renderable");
    }
}

bool AnimatedEntity::hasAnimationLooped() {
    return current_frame_ == static_cast<int>(frame_list_.size()) - 1;
}

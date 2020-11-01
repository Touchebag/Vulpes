#include <fstream>

#include "animation.h"
#include "utils/log.h"
#include "utils/file.h"
#include "utils/common.h"

AnimatedEntity::AnimatedEntity(std::weak_ptr<RenderableEntity> renderableEntity) :
    renderableEntity_(renderableEntity) {
}

std::unordered_map<std::string, util::Rectangle> AnimatedEntity::loadSpriteMap(const std::string& entity_name) {
    auto fs = File::openSpriteMapFile(entity_name);
    std::unordered_map<std::string, util::Rectangle> sprite_map;

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

        sprite_map.insert({name, {x_pos, y_pos, width, height}});
    }

    return sprite_map;
}

void AnimatedEntity::loadFrameList(
        const std::string& entity_name,
        const std::unordered_map<std::string, std::vector<std::string>>& list_frame_map) {
    // Map of frame name to texture coordinates
    auto sprite_map = loadSpriteMap(entity_name);

    for (auto it : list_frame_map) {
        // List of coordinates
        std::vector<util::Rectangle> coord_list;

        for (auto frame_it : it.second) {
            // Convert names to coords
            coord_list.push_back(sprite_map.at(frame_it));
        }

        sprite_sheet_map_.emplace(it.first, std::make_shared<std::vector<util::Rectangle>>(coord_list));
    }

    if (sprite_sheet_map_.size() < 1) {
        LOGE("No animations loaded");
        throw std::invalid_argument("");
    }

    current_frame_list_ = sprite_sheet_map_.begin()->second;
}

void AnimatedEntity::setFrameList(std::string animation_name) {
    try {
        if (animation_name.empty()) {
            return;
        }
        current_frame_list_ = sprite_sheet_map_.at(animation_name);
    } catch (std::out_of_range& e) {
        LOGE("Animation: name %s not found", animation_name.c_str());
        throw e;
    }
}

util::Rectangle AnimatedEntity::getSpriteRect() {
    try {
        if (current_frame_list_) {
            return current_frame_list_->at(current_frame_);
        } else {
            throw std::runtime_error("Current frame list empty");
        }
    } catch (std::out_of_range& e) {
        LOGE("Could not find sprite map entry");
        throw e;
    }
}

void AnimatedEntity::reloadFromJson(nlohmann::json j) {
    std::string main_entity_name = util::COMMON_ASSET_DIR;
    if (j.contains(util::MAIN_ENTITY_NAME)) {
        main_entity_name = j[util::MAIN_ENTITY_NAME].get<std::string>();
    }

    // Load the animation names as lists of frame names
    std::unordered_map<std::string, std::vector<std::string>> name_frames_map;

    // If there is a frame list directly, do not load from file
    if (j.contains("frame_list")) {
        nlohmann::json frame_names_array = j["frame_list"];

        std::vector<std::string> frame_list;
        for (auto it : frame_names_array) {
            frame_list.push_back(it.get<std::string>());
        }

        name_frames_map.emplace("default", frame_list);

        original_frame_list_ = frame_list;
    } else {
        // TODO Error handling
        nlohmann::json j_anim_list = File::loadAnimations(main_entity_name).value();

        for (auto animation : j_anim_list) {
            std::vector<std::string> frame_list;
            for (auto it : animation["frame_list"]) {
                frame_list.push_back(it.get<std::string>());
            }

            name_frames_map.emplace(animation["name"], frame_list);
        }

        original_frame_list_.reset();
    }

    loadFrameList(main_entity_name, name_frames_map);

    setRenderTexture();
}

std::optional<nlohmann::json> AnimatedEntity::outputToJson() {
    nlohmann::json j;

    if (original_frame_list_) {
        j["frame_list"] = original_frame_list_.value();
    }

    return j;
}

void AnimatedEntity::update() {
    if (current_frame_list_) {
        if (++current_frame_ >= static_cast<int>(current_frame_list_->size())) {
            current_frame_ = 0;
        }
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
    return current_frame_ == static_cast<int>(current_frame_list_->size()) - 1;
}

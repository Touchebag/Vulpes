#include <fstream>

#include "animation.h"
#include "utils/log.h"
#include "utils/file.h"
#include "utils/common.h"

AnimatedEntity::AnimatedEntity(std::weak_ptr<RenderableEntity> renderableEntity) :
    renderableEntity_(renderableEntity) {
}

std::unordered_map<std::string, util::Rectangle> AnimatedEntity::loadSpriteMap(File file_instance) {
    auto fs = file_instance.openSpriteMapFile();
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

std::shared_ptr<std::vector<AnimatedEntity::AnimationFrameData>> AnimatedEntity::loadAnimationFromJson(
        const nlohmann::json& j,
        const std::unordered_map<std::string, util::Rectangle>& sprite_map) {
    auto frame_data_list = std::make_shared<std::vector<AnimationFrameData>>();

    std::unordered_map<int, AnimationFrameData> meta_data;

    if (j.contains("meta_data")) {
        for (auto it : j["meta_data"]) {
            AnimationFrameData frame_data;

            if (it.contains("x_scale")) {
                frame_data.x_scale = it["x_scale"].get<float>();
            }

            if (it.contains("y_scale")) {
                frame_data.y_scale = it["y_scale"].get<float>();
            }

            if (it.contains("frame")) {
                meta_data.insert({it["frame"].get<int>(), frame_data});
            } else {
                LOGW("Animation meta data must contain frame number");
            }
        }
    }

    int i = 0;
    for (auto it : j["frame_list"]) {
        AnimationFrameData frame_data;

        if (meta_data.count(i)) {
            frame_data = meta_data.at(i);
        }

        frame_data.sprite_rectangle = (sprite_map.at(it.get<std::string>()));
        frame_data_list->push_back(frame_data);

        i++;
    }

    return frame_data_list;
}

void AnimatedEntity::setFrameList(std::string animation_name) {
    try {
        if (animation_name.empty()) {
            return;
        }
        current_frame_list_ = sprite_sheet_map_.at(animation_name);
        current_frame_ = 0;
    } catch (std::out_of_range& e) {
        LOGE("Animation: name %s not found", animation_name.c_str());
    }
}

AnimatedEntity::AnimationFrameData AnimatedEntity::getFrameData() {
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
    reloadFromJson(j, File());
}

void AnimatedEntity::reloadFromJson(nlohmann::json j, File file_instance) {
    auto sprite_map = loadSpriteMap(file_instance);

    // Load the animation names as lists of frame names
    std::unordered_map<std::string, std::vector<std::string>> name_frames_map;

    // If there is a frame list directly, do not load from file
    if (j.contains("frame_list")) {
        auto frame_data_list = loadAnimationFromJson(j, sprite_map);
        sprite_sheet_map_.emplace("default", frame_data_list);

        original_frame_list_ = j["frame_list"];
    } else {
        // TODO Error handling
        nlohmann::json j_anim_list = file_instance.loadAnimations().value();

        for (auto animation : j_anim_list) {
            auto frame_data_list = loadAnimationFromJson(animation, sprite_map);
            sprite_sheet_map_.emplace(animation["name"], frame_data_list);
        }

        original_frame_list_.reset();
    }

    if (sprite_sheet_map_.size() < 1) {
        LOGE("No animations loaded");
        throw std::invalid_argument("");
    }

    current_frame_list_ = sprite_sheet_map_.begin()->second;

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
        auto frame_data = getFrameData();

        auto sprite_rect = frame_data.sprite_rectangle;
        renderable->setTextureCoords(sprite_rect.x, sprite_rect.y, sprite_rect.width, sprite_rect.height);

        renderable->setScale(frame_data.x_scale, frame_data.y_scale);
    } else {
        LOGW("Animated: Missing renderable");
    }
}

bool AnimatedEntity::hasAnimationLooped() {
    return current_frame_ == static_cast<int>(current_frame_list_->size()) - 1;
}

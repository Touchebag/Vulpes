#include <fstream>

#include "animation.h"
#include "utils/log.h"
#include "utils/file.h"
#include "utils/common.h"
#include "components/component_store.h"

AnimatedEntity::AnimatedEntity(std::weak_ptr<ComponentStore> components) :
    Component(components) {
}

std::shared_ptr<AnimatedEntity> AnimatedEntity::createFromJson(nlohmann::json j, std::weak_ptr<ComponentStore> components, File file_instance) {
    auto ret_ptr = std::make_shared<AnimatedEntity>(components);

    ret_ptr->reloadFromJson(j, file_instance);

    return ret_ptr;
}

std::unordered_map<std::string, AnimatedEntity::AnimationFrameData> AnimatedEntity::loadSpriteMap(File file_instance, std::string file) {
    auto fs = file_instance.openSpriteMapFile(file);
    std::unordered_map<std::string, AnimationFrameData> sprite_map;

    std::vector<std::string> map_str(std::istream_iterator<std::string>{fs},
                                     std::istream_iterator<std::string>());

    for (auto it = map_str.begin(); it != map_str.end(); ++it) {
        AnimationFrameData frame_data;

        // Texture name should be same as sprite map file
        frame_data.texture = file;

        util::Rectangle rect;

        // File name
        std::string name = *it;
        ++it;
        // Skip equals sign
        ++it;
        // Positions
        rect.x = std::stoi(*it);
        ++it;
        rect.y = std::stoi(*it);
        ++it;
        // Size
        rect.width = std::stoi(*it);
        ++it;
        rect.height = std::stoi(*it);

        frame_data.sprite_rectangle = rect;

        sprite_map.insert({name, frame_data});
    }

    return sprite_map;
}

void AnimatedEntity::loadTexture(File file_instance, std::string file_path) {
    if (auto texture = file_instance.loadTexture(file_path)) {
        textures_.insert({file_path, std::make_shared<sf::Texture>(texture.value())});
    }
}

std::shared_ptr<std::vector<AnimatedEntity::AnimationFrameData>> AnimatedEntity::loadAnimationFromJson(
        const nlohmann::json& j,
        const std::unordered_map<std::string, AnimatedEntity::AnimationFrameData>& sprite_map) {
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
        AnimationFrameData frame_data = sprite_map.at(it.get<std::string>());

        if (meta_data.count(i)) {
            auto frame_md = meta_data.at(i);
            frame_data.x_scale = frame_md.x_scale;
            frame_data.y_scale = frame_md.y_scale;
        }

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

void AnimatedEntity::reloadFromJson(nlohmann::json j, File file_instance) {
    sprite_sheet_map_.clear();
    textures_.clear();

    std::unordered_map<std::string, AnimationFrameData> sprite_map;
    // TODO Move directory name completely into File class
    for (auto& file : file_instance.getDirContents("textures")) {
        auto path = file.path();
        if (path.extension() == ".txt") {
            for (auto it : loadSpriteMap(file_instance, path.stem().string())) {
                if (sprite_map.find(it.first) != sprite_map.end()) {
                    LOGW("Animation: entry \"%s\" already exists, overwriting", it.first.c_str());
                }
                sprite_map.insert(it);
            }
        } else {
            loadTexture(file_instance, path.stem().string());
        }
    }

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
    if (auto renderable = getComponent<RenderableEntity>()) {
        auto frame_data = getFrameData();

        renderable->setTexture(textures_.at(frame_data.texture));

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

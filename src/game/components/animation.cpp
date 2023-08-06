#include <fstream>

#include "animation.h"
#include "utils/log.h"
#include "utils/file.h"
#include "utils/common.h"
#include "components/component_store.h"

Animation::Animation(std::weak_ptr<ComponentStore> components) :
    Component(components) {
}

std::shared_ptr<Animation> Animation::createFromJson(nlohmann::json j, std::weak_ptr<ComponentStore> components, File file_instance) {
    auto ret_ptr = std::make_shared<Animation>(components);

    ret_ptr->reloadFromJson(j, file_instance);

    return ret_ptr;
}

std::unordered_map<std::string, std::pair< std::string, util::Rectangle>> Animation::loadSpriteMap(std::string file) {
    auto fs = File::openSpriteMapFile(file);
    std::unordered_map<std::string, std::pair<std::string, util::Rectangle>> sprite_map;

    std::vector<std::string> map_str(std::istream_iterator<std::string>{fs},
                                     std::istream_iterator<std::string>());

    // TODO Range check
    for (auto it = map_str.begin(); it < map_str.end(); ++it) {
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

        sprite_map.insert({name, {file, rect}});
    }

    return sprite_map;
}

void Animation::loadTexture(File file_instance, std::string file_path) {
    if (auto texture = file_instance.loadTexture(file_path)) {
        textures_.insert({file_path, std::make_shared<sf::Texture>(texture.value())});
    }
}

std::shared_ptr<std::vector<Animation::AnimationFrameData>> Animation::loadAnimationFromJson(
        const nlohmann::json& j,
        const std::unordered_map<std::string, std::pair<std::string, util::Rectangle>>& sprite_map) {
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

            if (it.contains("x_offset")) {
                frame_data.x_offset = it["x_offset"].get<int>();
            }

            if (it.contains("y_offset")) {
                frame_data.y_offset = it["y_offset"].get<int>();
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
        auto name = it.get<std::string>();

        auto sprite_rect = sprite_map.at(name);
        frame_data.texture = sprite_rect.first;
        frame_data.sprite_rectangle = sprite_rect.second;

        if (meta_data.count(i)) {
            auto frame_md = meta_data.at(i);
            frame_data.x_scale = frame_md.x_scale;
            frame_data.y_scale = frame_md.y_scale;
            frame_data.x_offset = frame_md.x_offset;
            frame_data.y_offset = frame_md.y_offset;
        }

        frame_data_list->push_back(frame_data);

        i++;
    }

    return frame_data_list;
}

void Animation::setFrameList(std::string animation_name) {
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

Animation::AnimationFrameData Animation::getFrameData() {
    try {
        if (current_frame_list_) {
            return current_frame_list_->at(static_cast<unsigned long long>(current_frame_));
        } else {
            throw std::runtime_error("Current frame list empty");
        }
    } catch (std::out_of_range& e) {
        LOGE("Could not find sprite map entry");
        throw;
    }
}

void Animation::reloadFromJson(nlohmann::json j, File file_instance) {
    if (File::isInRoot()) {
        throw std::invalid_argument("Animation: must be explicit entity directory");
    }

    sprite_sheet_map_.clear();
    textures_.clear();

    std::unordered_map<std::string, std::pair<std::string, util::Rectangle>> sprite_map;
    // TODO Move directory name completely into File class
    for (auto& file : file_instance.getDirContents("textures")) {
        auto path = file.path();
        if (path.extension() == ".txt") {
            for (auto it : loadSpriteMap(path.stem().string())) {
                if (sprite_map.find(it.first) != sprite_map.end()) {
                    LOGW("Animation: entry \"%s\" already exists, overwriting", it.first.c_str());
                }
                sprite_map.insert(it);
            }
        } else {
            loadTexture(file_instance, path.stem().string());
        }
    }

    // If there is a frame list directly, do not load from file
    if (j.contains("frame_list")) {
        auto frame_data_list = loadAnimationFromJson(j, sprite_map);
        sprite_sheet_map_.emplace("default", frame_data_list);

        original_frame_list_ = j["frame_list"];
    } else {
        // TODO Error handling
        nlohmann::json j_anim_list = file_instance.loadAnimations().value();

        for (auto [name, data] : j_anim_list.items()) {
            auto frame_data_list = loadAnimationFromJson(data, sprite_map);
            sprite_sheet_map_.emplace(name, frame_data_list);
        }

        original_frame_list_.reset();
    }

    if (sprite_sheet_map_.size() < 1) {
        LOGE("No animations loaded");
        throw std::invalid_argument("");
    }

    current_frame_list_ = sprite_sheet_map_.begin()->second;
}

std::optional<nlohmann::json> Animation::outputToJson() {
    nlohmann::json j;

    if (original_frame_list_) {
        j["frame_list"] = original_frame_list_.value();
    }

    return j;
}

void Animation::update() {
    if (current_frame_list_) {
        if (++current_frame_ >= static_cast<int>(current_frame_list_->size())) {
            current_frame_ = 0;
        }
    }

    setRenderTexture();
}

void Animation::setRenderTexture() {
    if (auto renderable = getComponent<Rendering>()) {
        auto frame_data = getFrameData();

        renderable->setTexture(textures_.at(frame_data.texture));

        auto sprite_rect = frame_data.sprite_rectangle;
        renderable->setTextureCoords(sprite_rect.x, sprite_rect.y, sprite_rect.width, sprite_rect.height);

        renderable->setScale(frame_data.x_scale, frame_data.y_scale);
        renderable->setOffset(frame_data.x_offset, frame_data.y_offset);
    } else {
        LOGW("Animated: Missing renderable");
    }
}

bool Animation::hasAnimationLooped() {
    return current_frame_ == static_cast<int>(current_frame_list_->size()) - 1;
}

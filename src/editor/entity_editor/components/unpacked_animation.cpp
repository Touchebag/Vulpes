#include "unpacked_animation.h"

#include "components/animation.h"

#include "utils/log.h"

UnpackedAnimation::UnpackedAnimation(const nlohmann::json& j) {

    for (auto it : j["frame_list"]) {
        AnimationFrameData data;
        data.name = it.get<std::string>();
        frame_list.push_back(data);
    }

    sprite_rect_map = Animation::loadSpriteMap("Player");

    if (j.contains("meta_data")) {
        for (auto it : j["meta_data"]) {
            int frame_number = it["frame"];

            auto& data = frame_list.at(frame_number);

            if (it.contains("x_offset")) {
                data.x_offset = it["x_offset"];
            }
            if (it.contains("y_offset")) {
                data.y_offset = it["y_offset"];
            }

            if (it.contains("x_scale")) {
                data.x_scale = it["x_scale"];
            }
            if (it.contains("y_scale")) {
                data.y_scale = it["y_scale"];
            }
        }
    }
}

nlohmann::json UnpackedAnimation::repack() {
    nlohmann::json j;

    std::vector<std::string> names;
    std::vector<nlohmann::json> meta_data;

    int i = 0;
    for (auto it : frame_list) {
        names.push_back(it.name);

        nlohmann::json frame_json;

        // Add frame number on every member in case none are defined
        if (it.x_offset) {
            frame_json["frame"] = i;
            frame_json["x_offset"] = it.x_offset.value();
        }
        if (it.y_offset) {
            frame_json["frame"] = i;
            frame_json["y_offset"] = it.y_offset.value();
        }

        if (it.x_scale) {
            frame_json["frame"] = i;
            frame_json["x_scale"] = it.x_scale.value();
        }
        if (it.y_scale) {
            frame_json["frame"] = i;
            frame_json["y_scale"] = it.y_scale.value();
        }

        if (!frame_json.empty()) {
            meta_data.push_back(frame_json);
        }

        i++;
    }

    j["frame_list"] = names;
    if (!meta_data.empty()) {
        j["meta_data"] = meta_data;
    }

    return j;
}

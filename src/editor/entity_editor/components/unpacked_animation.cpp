#include "unpacked_animation.h"

#include "utils/log.h"

UnpackedAnimation::UnpackedAnimation(const nlohmann::json& j) {
    for (auto it : j["frame_list"]) {
        frame_list.push_back(it.get<std::string>());
    }

    if (j.contains("meta_data")) {
        for (auto it : j["meta_data"]) {
            AnimationMetaData data;

            int frame_number = it["frame"];

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

            meta_data.insert({frame_number, data});
        }
    }
}

nlohmann::json UnpackedAnimation::repack() {
    nlohmann::json j;

    j["frame_list"] = frame_list;

    if (!meta_data.empty()) {
        std::vector<nlohmann::json> meta_json;

        for (auto it : meta_data) {
            nlohmann::json frame_json;

            frame_json["frame"] = it.first;

            if (it.second.x_offset) {
                frame_json["x_offset"] = it.second.x_offset.value();
            }
            if (it.second.y_offset) {
                frame_json["y_offset"] = it.second.y_offset.value();
            }

            if (it.second.x_scale) {
                frame_json["x_scale"] = it.second.x_scale.value();
            }
            if (it.second.y_scale) {
                frame_json["y_scale"] = it.second.y_scale.value();
            }

            meta_json.push_back(frame_json);
        }

        j["meta_data"] = meta_json;
    }

    return j;
}

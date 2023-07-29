#pragma once

#include <optional>

#include <nlohmann/json.hpp>

#include "utils/common.h"

struct UnpackedAnimation {
    explicit UnpackedAnimation(const nlohmann::json& j);
    nlohmann::json repack();

    std::string texture;
    util::Rectangle sprite_rectangle;

    std::string name;

    std::vector<std::string> frame_list;

    struct AnimationMetaData {
        std::optional<int> x_offset;
        std::optional<int> y_offset;
        std::optional<double> x_scale;
        std::optional<double> y_scale;
    };

    std::map<int, AnimationMetaData> meta_data;
};

#pragma once

#include <optional>

#include <nlohmann/json.hpp>

#include "utils/common.h"

struct UnpackedAnimation {
    explicit UnpackedAnimation(const std::string& name, const nlohmann::json& j);
    nlohmann::json repack();

    std::string name;

    std::unordered_map<std::string, std::pair<std::string, util::Rectangle>> sprite_rect_map;

    struct AnimationFrameData {
        std::string name;
        std::optional<int> x_offset;
        std::optional<int> y_offset;
        std::optional<double> x_scale;
        std::optional<double> y_scale;
    };

    std::vector<AnimationFrameData> frame_list;
};

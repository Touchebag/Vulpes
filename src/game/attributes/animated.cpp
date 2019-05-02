#include "animated.h"
#include "log.h"

#include <fstream>

typedef std::unordered_map<std::string, std::pair<util::Point, util::Point>> point_map;

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

        sprite_sheet_map_.insert({name, {{x_pos, y_pos}, {width, height}}});
    }
}

std::pair<util::Point, util::Point> AnimatedEntity::getSpriteRect(std::string sprite) {
    // TODO This is origin and size, not two coordinates
    // Change type to reflect that
    return sprite_sheet_map_.at(sprite);
}

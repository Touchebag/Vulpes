#pragma once

#include <unordered_map>
#include <SFML/Graphics.hpp>

#include "utils/common.h"

// This class keeps the sprite sheet coordinates of the currently loaded texture
class AnimatedEntity {
  typedef std::unordered_map<std::string, std::pair<util::Point, util::Point>> point_map;

  public:
    void load_sprite_map(const std::string& path);

    // Returns a pair of the top-left and bottom-right points of requested texture
    std::pair<util::Point, util::Point> get_sprite_rect(std::string);

  private:
    point_map sprite_sheet_map_;
};

#pragma once

#include <unordered_map>
#include <SFML/Graphics.hpp>

#include "utils/common.h"

// This class keeps the sprite sheet coordinates of the currently loaded texture
class AnimatedEntity {
  typedef std::unordered_map<std::string, std::pair<util::Point, util::Point>> point_map;

  public:
    virtual void loadSpriteMap(const std::string& path) final;

    // Returns a pair of the top-left and bottom-right points of requested texture
    virtual std::pair<util::Point, util::Point> getSpriteRect(std::string) final;

  private:
    point_map sprite_sheet_map_;
};
